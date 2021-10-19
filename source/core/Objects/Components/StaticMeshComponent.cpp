#include "StaticMeshComponent.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanHighLevel.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Resources/ResourceManager.h"
#include "GlobalVariables.h"

namespace Engine
{
    void StaticMeshComponent::Create(std::shared_ptr<ResourceManager> resourceMgr)
    {
        RenderObject::Create(resourceMgr);
        m_pUniform = std::make_shared<UniformBuffer<FUniformData>>();
        m_pUniform->Create(UDevice, USwapChain->GetImages().size());

        m_pStaticMesh = resourceMgr->Get<MeshBase>(fBindings.vMeshes.front());
    }

    void StaticMeshComponent::ReCreate()
    {
        RenderObject::ReCreate();
        m_pUniform->ReCreate(UDevice, USwapChain->GetImages().size());
        m_pStaticMesh->ReCreate();
    }

    void StaticMeshComponent::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);

        uint32_t imageIndex = URenderer->GetImageIndex();

        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        auto transform = GetTransform();
        FUniformData ubo{};
        ubo.model = transform.GetModel();
        ubo.view = camera->GetView();
        ubo.projection = camera->GetProjection();
        //ubo.normalMatrix = glm::transpose(glm::inverse(ubo.model));

        ubo.viewPosition = glm::vec4(camera->GetTransform().pos, 1.0);
        ubo.lightPosition = glm::vec4(GlobalVariables::lightPosition[0], GlobalVariables::lightPosition[1], GlobalVariables::lightPosition[2], 1.0);
        m_pUniform->UpdateUniformBuffer(UDevice, imageIndex, ubo);
    }

    void StaticMeshComponent::Render(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        RenderObject::Render(commandBuffer, imageIndex);

        m_pStaticMesh->Update(imageIndex, m_pUniform->GetUniformBuffer(imageIndex));
        m_pStaticMesh->Bind(commandBuffer, imageIndex);

    }

    void StaticMeshComponent::Cleanup()
    {
        RenderObject::Cleanup();
        m_pUniform->Cleanup(UDevice);
        m_pStaticMesh->Cleanup();
    }

    void StaticMeshComponent::Destroy()
    {
        RenderObject::Destroy();
        m_pUniform->Cleanup(UDevice);
    }
}