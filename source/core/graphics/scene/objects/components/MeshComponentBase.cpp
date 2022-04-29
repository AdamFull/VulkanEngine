#include "MeshComponentBase.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/VulkanRenderSystem.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/Meshes/VulkanMesh.h"
#include "graphics/Scene/Objects/Components/Camera/CameraManager.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;
using namespace Engine::Core;

void MeshComponentBase::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    ComponentBase::Create(renderPass, subpass);

    if (m_pMesh)
    {
        m_pMesh->Create(renderPass, subpass);
    }
}

void MeshComponentBase::ReCreate()
{
    ComponentBase::ReCreate();

    if (m_pMesh)
    {
        m_pMesh->ReCreate();
    }
}

void MeshComponentBase::Update(float fDeltaTime)
{
    ComponentBase::Update(fDeltaTime);

    if (m_pMesh)
    {

        
    }
}

void MeshComponentBase::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    ComponentBase::Render(commandBuffer, imageIndex);

    if (m_pMesh)
    {
        auto camera = CameraManager::getInstance()->GetCurrentCamera();
        auto transform = GetTransform();
        FUniformData ubo{};
        ubo.model = transform.GetModel();
        ubo.view = camera->GetView();
        ubo.projection = camera->GetProjection();

        for(uint32_t i = 0; i < m_vInstances.size(); i++)
            ubo.instancePos[i] = m_vInstances.at(i);
        
        auto instanceCount = m_vInstances.size();
        m_pMesh->Render(commandBuffer, imageIndex, ubo, instanceCount == 0 ? 1 : instanceCount);
    }
}

void MeshComponentBase::Cleanup()
{
    ComponentBase::Cleanup();

    if (m_pMesh)
    {
        m_pMesh->Cleanup();
    }
}

void MeshComponentBase::Destroy()
{
    ComponentBase::Destroy();
}