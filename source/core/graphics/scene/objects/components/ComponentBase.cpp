#include "ComponentBase.h"

using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

void ComponentBase::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    RenderObject::Create(renderPass, subpass);
}

void ComponentBase::ReCreate()
{
    RenderObject::ReCreate();
}

void ComponentBase::Update(float fDeltaTime)
{
    RenderObject::Update(fDeltaTime);
}

void ComponentBase::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    RenderObject::Render(commandBuffer, imageIndex);
}

void ComponentBase::Cleanup()
{
    RenderObject::Cleanup();
}

void ComponentBase::Destroy()
{
    RenderObject::Destroy();
}