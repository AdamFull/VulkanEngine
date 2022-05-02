#include "graphics/scene/objects/components/light/LightComponent.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene;

void CLightComponent::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CComponentBase::create(renderPass, subpass);
}

void CLightComponent::reCreate()
{
    CComponentBase::reCreate();
}

void CLightComponent::update(float fDeltaTime)
{
    CComponentBase::update(fDeltaTime);
}

void CLightComponent::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CComponentBase::render(commandBuffer, imageIndex);
}

FLight CLightComponent::getLight()
{
    FLight light{};
    light.position = getPosition();
    light.color = m_vColor;
    light.radius = m_fAttenuation;
    return light;
}