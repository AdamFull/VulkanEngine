#include "LightComponent.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "GlobalVariables.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponent::update(float fDeltaTime)
{
	timer += fDeltaTime * 0.025f;
	auto& transform = pParent->getLocalTransform();
	float angle = glm::radians(timer * 360.0f);
	float radius = 20.0f;
	//transform.rot = glm::vec3(cos(angle) * radius, -radius, sin(angle) * radius);
}

void CLightComponent::setType(ELightSourceType type)
{
    lightData.type = static_cast<int32_t>(type);
}

ELightSourceType CLightComponent::getType()
{
    return static_cast<ELightSourceType>(lightData.type);
}

void CLightComponent::setColor(const glm::vec3& color)
{
    lightData.color = color;
}

const glm::vec3& CLightComponent::getColor()
{
    return lightData.color;
}

void CLightComponent::setInnerAngle(float angle)
{
    lightData.innerConeAngle = angle;
}

const float CLightComponent::getInnerAngle()
{
    return lightData.innerConeAngle;
}


void CLightComponent::setOuterAngle(float angle)
{
    lightData.outerConeAngle = angle;
}

const float CLightComponent::getOuterAngle()
{
    return lightData.outerConeAngle;
}


void CLightComponent::setIntencity(float intencity)
{
    lightData.intencity = intencity;
}

const float CLightComponent::getIntencity()
{
    return lightData.intencity;
}

FLight& CLightComponent::getLight()
{
    auto transform = pParent->getTransform();
    lightData.position = transform.getPosition();
    lightData.direction = transform.getRotation();

    if(lightData.type == 1)
        updateCascades();

    return lightData;
}

/*
		Calculate frustum split depths and matrices for the shadow map cascades
		Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
	*/
void CLightComponent::updateCascades()
{
    std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
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

	// Calculate split depths based on view camera frustum
	// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
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
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) 
	{
		float splitDist = cascadeSplits[i];

		glm::vec3 frustumCorners[8] = 
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
		glm::mat4 invCam = glm::inverse(projection * view);
		for (uint32_t i = 0; i < 8; i++) 
		{
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = invCorner / invCorner.w;
		}

		for (uint32_t i = 0; i < 4; i++) 
		{
			glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t i = 0; i < 8; i++)
			frustumCenter += frustumCorners[i];
		frustumCenter /= 8.0f;

		float radius = 0.0f;
		for (uint32_t i = 0; i < 8; i++) 
		{
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::vec3 lightDir = glm::normalize(-lightData.direction);
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

		// Store split distance and matrix in cascade
		lightData.aCascadeSplits[i] = (nearClip + splitDist * clipRange) * -1.0f;
		lightData.aCascadeViewProjMat[i] = glm::scale(lightOrthoMatrix * lightViewMatrix, glm::vec3(-1.0, 1.0, -1.0));

		lastSplitDist = cascadeSplits[i];
	}
}