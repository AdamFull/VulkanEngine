#include "GLTFSceneObjectComponent.h"

namespace Engine
{
    void GLTFSceneObjectComponent::Create(std::shared_ptr<ResourceManager> resourceMgr)
    {
        RenderObject::Create(resourceMgr);
    }

    void GLTFSceneObjectComponent::ReCreate()
    {
        RenderObject::ReCreate();
    }

    void GLTFSceneObjectComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);
    }

    void GLTFSceneObjectComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void GLTFSceneObjectComponent::Cleanup()
    {
        RenderObject::Cleanup();
    }

    void GLTFSceneObjectComponent::Destroy()
    {
        RenderObject::Destroy();
    }

    void GLTFSceneObjectComponent::SetIndex(uint32_t index)
    {
        m_iIndex = index;
    }
}