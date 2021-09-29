#include "StaticMeshComponent.h"
#include "VulkanDevice.h"
#include "VulkanHighLevel.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Camera/CameraManager.h"
#include "Camera/Camera.h"

namespace Engine
{
    void StaticMeshComponent::Create()
    {
        RenderObject::Create();
        m_pStaticMesh = std::make_unique<StaticMesh>();
        m_pStaticMesh->Create("../../assets/meshes/viking_room.obj");
    }

    void StaticMeshComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void StaticMeshComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);
        m_pMaterial->Update(imageIndex);
        m_pMaterial->Bind(commandBuffer, imageIndex);
        
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        auto matrix = camera->GetProjection() * camera->GetView() * m_transform.GetModel();
        VulkanHighLevel::GetInstance()->GetUniformBuffer()->UpdateUniformBuffer(VulkanHighLevel::GetInstance()->GetDevice(), imageIndex, matrix);

        m_pStaticMesh->Update(imageIndex);
        m_pStaticMesh->Bind(commandBuffer, imageIndex);
    }
}