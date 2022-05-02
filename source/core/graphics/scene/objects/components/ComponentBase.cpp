#include "ComponentBase.h"

using namespace Engine::Core::Scene;
using namespace Engine::Resources;

void CComponentBase::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CRenderObject::create(renderPass, subpass);
}

void CComponentBase::reCreate()
{
    CRenderObject::reCreate();
}

void CComponentBase::update(float fDeltaTime)
{
    CRenderObject::update(fDeltaTime);
}

void CComponentBase::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CRenderObject::render(commandBuffer, imageIndex);
}

void CComponentBase::cleanup()
{
    CRenderObject::cleanup();
}

void CComponentBase::destroy()
{
    CRenderObject::destroy();
}