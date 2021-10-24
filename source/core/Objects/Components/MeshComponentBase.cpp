#include "MeshComponentBase.h"
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
    void MeshComponentBase::Create(std::shared_ptr<ResourceManager> resourceMgr)
    {
        ComponentBase::Create(resourceMgr);

        if (m_pMesh)
        {
            m_pUniform = std::make_shared<UniformBuffer<FUniformData>>();
            m_pUniform->Create(UDevice, USwapChain->GetImages().size());
            m_pMesh->Create();
        }
    }

    void MeshComponentBase::ReCreate()
    {
        ComponentBase::ReCreate();

        if (m_pMesh)
        {
            m_pUniform->ReCreate(UDevice, USwapChain->GetImages().size());
            m_pMesh->ReCreate();
        }
    }

    void MeshComponentBase::Update(float fDeltaTime)
    {
        ComponentBase::Update(fDeltaTime);

        if (m_pMesh)
        {
            uint32_t imageIndex = URenderer->GetImageIndex();

            auto camera = CameraManager::GetInstance()->GetCurrentCamera();
            auto transform = GetTransform();
            FUniformData ubo{};
            ubo.model = transform.GetModel();
            ubo.view = camera->GetView();
            ubo.projection = camera->GetProjection();
            ubo.normal = glm::transpose(glm::inverse(ubo.view * ubo.model));

            ubo.viewPosition = glm::vec4(camera->GetTransform().pos, 1.0);
            ubo.lightPosition = glm::vec4(GlobalVariables::lightPosition[0], GlobalVariables::lightPosition[1], GlobalVariables::lightPosition[2], 1.0);
            m_pUniform->UpdateUniformBuffer(UDevice, imageIndex, ubo);
        }
    }

    void MeshComponentBase::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
    {
        ComponentBase::Render(commandBuffer, imageIndex);

        if (m_pMesh)
        {
            m_pMesh->Update(imageIndex, m_pUniform->GetUniformBuffer(imageIndex));
            m_pMesh->Bind(commandBuffer, imageIndex);
        }
    }

    void MeshComponentBase::Cleanup()
    {
        ComponentBase::Cleanup();

        if (m_pMesh)
        {
            m_pUniform->Cleanup(UDevice);
            m_pMesh->Cleanup();
        }
    }

    void MeshComponentBase::Destroy()
    {
        ComponentBase::Destroy();
        if (m_pMesh)
        {
            m_pUniform->Cleanup(UDevice);
        }
    }
}