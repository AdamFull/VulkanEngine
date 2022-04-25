#include "graphics/scene/objects/components/light/LightComponent.h"

using namespace Engine::Resources;
using namespace Engine::Core;
using namespace Engine::Core::Scene::Objects::Components::Light;

void LightComponent::Create(std::shared_ptr<ResourceManager> resourceMgr)
{
    ComponentBase::Create(resourceMgr);
}

void LightComponent::ReCreate()
{
    ComponentBase::ReCreate();
}

void LightComponent::Update(float fDeltaTime)
{
    ComponentBase::Update(fDeltaTime);
}

void LightComponent::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    ComponentBase::Render(commandBuffer, imageIndex);
}

FLight LightComponent::GetLight()
{
    FLight light{};
    light.position = GetPosition();
    light.color = m_vColor;
    light.radius = m_fAttenuation;
    return light;
}