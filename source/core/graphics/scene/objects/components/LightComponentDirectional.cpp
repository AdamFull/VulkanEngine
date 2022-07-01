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
	updateCascadesEx();
	//updateCascades();
}

glm::mat4 CLightComponentDirectional::getLightSpaceMatrixEx(const float lastSplitDist, const float splitDist)
{
	auto transform = pParent->getTransform();
    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();

	auto projection = camera->getProjection();
	auto view = camera->getView();

	auto frustumCorners = getFrostumCornersWorldSpaceEx(projection, view, splitDist, lastSplitDist);

	// Get frustum center
	glm::vec3 frustumCenter = glm::vec3(0.0f);
	for (uint32_t i = 0; i < frustumCorners.size(); i++)
		frustumCenter += frustumCorners[i];
	frustumCenter /= frustumCorners.size();

	float radius = 0.0f;
	for (uint32_t i = 0; i < frustumCorners.size(); i++) 
	{
		float distance = glm::length(frustumCorners[i] - frustumCenter);
		radius = glm::max(radius, distance);
	}
	radius = std::ceil(radius * 16.0f) / 16.0f;

	glm::vec3 maxExtents = glm::vec3(radius);
	glm::vec3 minExtents = -maxExtents;

	auto cascadeExtents = (maxExtents - minExtents);
	glm::vec3 lightDir = glm::normalize(-transform.rot);
	glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);

	glm::mat4 shadowMatrix = lightOrthoMatrix * lightViewMatrix;
	glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	shadowOrigin = shadowMatrix * shadowOrigin;
	shadowOrigin = shadowOrigin * static_cast<float>(SHADOW_MAP_RESOLUTION) / 2.0f;
	
	glm::vec4 roundedOrigin = glm::round(shadowOrigin);
	glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
	roundOffset = roundOffset * 2.0f / static_cast<float>(SHADOW_MAP_RESOLUTION);
	roundOffset.z = 0.0f;
	roundOffset.w = 0.0f;

	glm::mat4 shadowProj = lightOrthoMatrix;
	shadowProj[3] += roundOffset;
	lightOrthoMatrix = shadowProj;

	return lightOrthoMatrix * lightViewMatrix;
}

std::array<glm::vec3, 8> CLightComponentDirectional::getFrostumCornersWorldSpaceEx(const glm::mat4& proj, const glm::mat4& view, float splitDist, float lastSplitDist)
{
	std::array<glm::vec3, 8> frustumCorners = 
	{
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
	glm::mat4 invCam = glm::inverse(proj * view);
	for (uint32_t i = 0; i < 8; i++) {
		glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
		frustumCorners[i] = invCorner / invCorner.w;
	}

	for (uint32_t i = 0; i < 4; i++) {
		glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
		frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
		frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
	}
	return frustumCorners;
}

/*
	Calculate frustum split depths and matrices for the shadow map cascades
	Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
*/
void CLightComponentDirectional::updateCascadesEx()
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

		// Store split distance and matrix in cascade
		this->aCascadeSplits[i] = (nearClip + splitDist * clipRange) * -1.f;
		this->aCascadeViewProjMat[i] = glm::scale(getLightSpaceMatrixEx(lastSplitDist, splitDist), glm::vec3(-1.0, -1.0, -1.0));

		lastSplitDist = cascadeSplits[i];
	}
}

void CLightComponentDirectional::updateCascades()
{
	auto transform = pParent->getTransform();
    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
	auto& cameraTransform = cameraNode->getLocalTransform();

	float nearClip = camera->getNearPlane();
	float farClip = camera->getFarPlane();
	auto aspect = CDevice::inst()->getAspect(true);

	float lambda = GlobalVariables::cascadeSplitLambda;
	float ratio = farClip/nearClip;
	aFrustum[0].neard = nearClip;

	for(int32_t i=1; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float si = i / (float)SHADOW_MAP_CASCADE_COUNT;

		aFrustum[i].neard = lambda * (nearClip * glm::pow(ratio, si)) + (1.0f - lambda) * (nearClip + (farClip - nearClip) * si);
		aFrustum[i-1].fard = aFrustum[i].neard * 1.005f;
	}
	aFrustum[SHADOW_MAP_CASCADE_COUNT-1].fard = farClip;

	for(int32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		auto& f = aFrustum[i];
		f.ratio = aspect;
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::cross(cameraTransform.rot, up);

		glm::vec3 fc = cameraTransform.pos + cameraTransform.rot * f.fard;
		glm::vec3 nc = cameraTransform.pos + cameraTransform.rot * f.neard;

		right = glm::normalize(right);
		up = glm::normalize(glm::cross(right, cameraTransform.rot));

		// these heights and widths are half the heights and widths of
		// the near and far plane rectangles
		float near_height = glm::tan(f.fov/2.0f) * f.neard;
		float near_width = near_height * f.ratio;
		float far_height = glm::tan(f.fov/2.0f) * f.fard;
		float far_width = far_height * f.ratio;

		f.point[0] = nc - up*near_height - right*near_width;
		f.point[1] = nc + up*near_height - right*near_width;
		f.point[2] = nc + up*near_height + right*near_width;
		f.point[3] = nc - up*near_height + right*near_width;

		f.point[4] = fc - up*far_height - right*far_width;
		f.point[5] = fc + up*far_height - right*far_width;
		f.point[6] = fc + up*far_height + right*far_width;
		f.point[7] = fc - up*far_height + right*far_width;

		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t j = 0; j < f.point.size(); j++)
			frustumCenter += f.point[j];
		frustumCenter /= f.point.size();

		float radius = 0.0f;
		for (uint32_t j = 0; j < f.point.size(); j++) 
		{
			float distance = glm::length(f.point[j] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		auto cascadeExtents = (maxExtents - minExtents);
		glm::vec3 lightDir = glm::normalize(-transform.rot);
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);

		this->aCascadeSplits[i] = f.fard * -1.f;
		this->aCascadeViewProjMat[i] = lightOrthoMatrix * lightViewMatrix;
	}
}