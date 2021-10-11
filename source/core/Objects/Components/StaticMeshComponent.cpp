#include "StaticMeshComponent.h"
#include "Resources/ResourceManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Camera/CameraManager.h"
#include "Camera/Camera.h"

namespace Engine
{
    void StaticMeshComponent::Create()
    {
        ComponentBase::Create();
        pResourceManager->Load("../../assets/resources.json");
        m_pStaticMesh = pResourceManager->Get<MeshBase>("femalebody_obj");
    }

    void StaticMeshComponent::ReCreate()
    {
        ComponentBase::ReCreate();
        m_pStaticMesh->ReCreate();
    }

    void StaticMeshComponent::Update(float fDeltaTime)
    {
        ComponentBase::Update(fDeltaTime);
    }

    void StaticMeshComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        ComponentBase::Render(commandBuffer, imageIndex);
        m_pStaticMesh->Update(imageIndex, UUniform->GetUniformBuffer(imageIndex));
        m_pStaticMesh->Bind(commandBuffer, imageIndex);
    }

    void StaticMeshComponent::Cleanup()
    {
        ComponentBase::Cleanup();
        m_pStaticMesh->Cleanup();
    }

    void StaticMeshComponent::Destroy()
    {
        ComponentBase::Destroy();
    }
}