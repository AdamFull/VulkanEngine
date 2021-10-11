#include "ComponentBase.h"
#include "Resources/ResourceManager.h"

namespace Engine
{
    void ComponentBase::Create()
    {
        RenderObject::Create();
        pResourceManager = std::make_shared<ResourceManager>();
    }

    void ComponentBase::ReCreate()
    {
        RenderObject::ReCreate();
    }

    void ComponentBase::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);
    }

    void ComponentBase::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void ComponentBase::Cleanup()
    {
        RenderObject::Cleanup();
    }

    void ComponentBase::Destroy()
    {
        RenderObject::Destroy();
        pResourceManager->DestroyAll();
    }

}