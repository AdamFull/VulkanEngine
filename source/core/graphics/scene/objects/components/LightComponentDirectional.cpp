#include "LightComponentDirectional.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "GlobalVariables.h"

using namespace engine::core::scene;
using namespace engine::resources;

//Based on https://learnopengl.com/Guest-Articles/2021/CSM

void CLightComponentDirectional::create()
{
    CLightComponent::create();
}

void CLightComponentDirectional::update(float fDeltaTime)
{
    CLightComponent::update(fDeltaTime);

	auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();

	auto farPlane = camera->getFarPlane();
	auto nearPlane = camera->getNearPlane();

	float clipRange = farPlane - nearPlane;

	float minZ = nearPlane;
	float maxZ = nearPlane + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	//this->aCascadeSplits = { farPlane / 64.0f, farPlane / 32.0f, farPlane / 16.0f, farPlane / 2.0f };

	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) 
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = GlobalVariables::cascadeSplitLambda * (log - uniform) + uniform;
		this->aCascadeSplits[i] = (d - nearPlane) / clipRange;
		this->aCascadeSplits[i] = (nearPlane + this->aCascadeSplits[i] * clipRange);
	}
    
	for(uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		if (i == 0)
            this->aCascadeViewProjMat[i] = getLightSpaceMatrix(nearPlane, this->aCascadeSplits[i]);
        else if (i < SHADOW_MAP_CASCADE_COUNT - 1)
            this->aCascadeViewProjMat[i] = getLightSpaceMatrix(this->aCascadeSplits[i - 1], this->aCascadeSplits[i]);
        else
            this->aCascadeViewProjMat[i] = getLightSpaceMatrix(this->aCascadeSplits[i - 1], farPlane);
	}

	//updateCascades();
}

glm::mat4 CLightComponentDirectional::getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
	auto transform = pParent->getTransform();
	auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();

	auto cameraFov = camera->getFieldOfView();
	auto aspect = CDevice::inst()->getAspect(true);
	auto projection = glm::perspective(glm::radians(cameraFov), aspect, nearPlane, farPlane);
	auto view = camera->getView();

	auto corners = getFrustumCornersWorldSpace(projection, view);

	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const auto& v : corners)
		center += glm::vec3(v);
	center /= corners.size();

	const auto lightView = glm::lookAt(center + transform.rot, center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();
	for (const auto& v : corners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// Tune this parameter according to the scene
	constexpr float zMult = 10.0f;
	if (minZ < 0)
		minZ *= zMult;
	else
		minZ /= zMult;

	if (maxZ < 0)
		maxZ /= zMult;
	else
		maxZ *= zMult;
	
	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

std::vector<glm::vec4> CLightComponentDirectional::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    const auto inv = glm::inverse(proj * view);
    
    std::vector<glm::vec4> frustumCorners;
    for (uint32_t x = 0; x < 2; ++x)
    {
        for (uint32_t y = 0; y < 2; ++y)
        {
            for (uint32_t z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f,  2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    return frustumCorners;
}

/*
	Calculate frustum split depths and matrices for the shadow map cascades
	Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
*/
void CLightComponentDirectional::updateCascades()
{
    std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
    auto transform = pParent->getTransform();
    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();

	auto projection = camera->getProjection();
	auto view = camera->getView();

    float nearClip = camera->getNearPlane();
	float farClip = camera->getFarPlane();
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) 
	{
		float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = GlobalVariables::cascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearClip) / clipRange;
	}

		// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
		float splitDist = cascadeSplits[i];

		glm::vec3 frustumCorners[8] = {
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3( 1.0f,  1.0f, -1.0f),
			glm::vec3( 1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
		};

		// Project frustum corners into world space
		glm::mat4 invCam = glm::inverse(projection * view);
		for (uint32_t i = 0; i < 8; i++) {
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint32_t i = 0; i < 4; i++) {
			glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t i = 0; i < 8; i++) {
			frustumCenter += frustumCorners[i];
		}
		frustumCenter /= 8.0f;

		float radius = 0.0f;
		for (uint32_t i = 0; i < 8; i++) {
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::vec3 lightDir = glm::normalize(-transform.rot);
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

		// Store split distance and matrix in cascade
		this->aCascadeSplits[i] = (nearClip + splitDist * clipRange);
		this->aCascadeViewProjMat[i] = lightOrthoMatrix * lightViewMatrix;

		lastSplitDist = cascadeSplits[i];
	}
}