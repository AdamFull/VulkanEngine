#include "StaticMeshComponent.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Resources/ResourceManager.h"

namespace Engine
{
    void StaticMeshComponent::Create(std::shared_ptr<ResourceManager> resourceMgr)
    {
        RenderObject::Create(resourceMgr);
        m_pStaticMesh = resourceMgr->Get<MeshBase>(fBindings.vMeshes.front());
    }

    void StaticMeshComponent::ReCreate()
    {
        RenderObject::ReCreate();
        m_pStaticMesh->ReCreate();
    }

    void StaticMeshComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void StaticMeshComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);
        m_pStaticMesh->Update(imageIndex, UUniform->GetUniformBuffer(imageIndex));
        m_pStaticMesh->Bind(commandBuffer, imageIndex);
    }

    void StaticMeshComponent::Cleanup()
    {
        RenderObject::Cleanup();
        m_pStaticMesh->Cleanup();
    }

    void StaticMeshComponent::Destroy()
    {
        RenderObject::Destroy();
    }
}