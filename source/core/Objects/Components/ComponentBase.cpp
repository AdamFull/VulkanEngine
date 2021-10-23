#include "ComponentBase.h"

namespace Engine
{
    void ComponentBase::Create(std::shared_ptr<ResourceManager> resourceMgr)
    {
        RenderObject::Create(resourceMgr);
    }

    void ComponentBase::ReCreate()
    {
        RenderObject::ReCreate();
    }

    void ComponentBase::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void ComponentBase::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
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

}