#include "LightComponent.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "GlobalVariables.h"

using namespace engine::core::scene;
using namespace engine::resources;

void CLightComponent::create()
{
	CSceneComponent::create();
}

void CLightComponent::update(float fDeltaTime)
{
	CSceneComponent::update(fDeltaTime);
}

void CLightComponent::setType(ELightSourceType type)
{
	eType = type;
}

ELightSourceType CLightComponent::getType()
{
	return eType;
}

const glm::vec3 CLightComponent::getPosition()
{
	auto transform = pParent.lock()->getTransform();
	return transform.pos;
}

const glm::vec3 CLightComponent::getDirection()
{
	auto transform = pParent.lock()->getTransform();
	return transform.rot;
}


void CLightComponent::setColor(const glm::vec3& color)
{
	this->color = color;
}

const glm::vec3& CLightComponent::getColor()
{
	return color;
}


void CLightComponent::setInnerAngle(float angle)
{
	innerConeAngle = angle;
}

const float& CLightComponent::getInnerAngle()
{
	return innerConeAngle;
}


void CLightComponent::setOuterAngle(float angle)
{
	outerConeAngle = angle;
}

const float& CLightComponent::getOuterAngle()
{
	return outerConeAngle;
}


void CLightComponent::setIntencity(float intencity)
{
	this->intencity = intencity;
}

const float& CLightComponent::getIntencity()
{
	return intencity;
}

void CLightComponent::setRadius(float radius)
{
	this->radius = radius;
}

const float& CLightComponent::getRadius()
{
	return radius;
}


const glm::mat4& CLightComponent::getShadowView()
{
	return shadowView;
}

const std::array<glm::mat4, 6>& CLightComponent::getShadowViews()
{
	return shadowViews;
}

const std::array<float, SHADOW_MAP_CASCADE_COUNT>& CLightComponent::getCascadeSplits()
{
	return aCascadeSplits;
}

const std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT>& CLightComponent::getCascadeViews()
{
	return aCascadeViewProjMat;
}
