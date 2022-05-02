#include "MeshComponentBase.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/VulkanRenderSystem.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/Meshes/VulkanMesh.h"
#include "graphics/Scene/Objects/Components/Camera/CameraManager.h"
#include "resources/ResourceManager.h"
#include "GlobalVariables.h"

using namespace Engine::Core::Scene;
using namespace Engine::Resources;
using namespace Engine::Core;

void CMeshComponentBase::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CComponentBase::create(renderPass, subpass);

    if (m_pMesh)
    {
        m_pMesh->create(renderPass, subpass);
    }
}

void CMeshComponentBase::reCreate()
{
    CComponentBase::reCreate();

    if (m_pMesh)
    {
        m_pMesh->reCreate();
    }
}

void CMeshComponentBase::update(float fDeltaTime)
{
    CComponentBase::update(fDeltaTime);

    if (m_pMesh)
    {

        
    }
}

void CMeshComponentBase::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CComponentBase::render(commandBuffer, imageIndex);

    if (m_pMesh)
    {
        auto camera = CCameraManager::getInstance()->getCurrentCamera();
        auto transform = getTransform();
        FUniformData ubo{};
        ubo.model = transform.getModel();
        ubo.view = camera->getView();
        ubo.projection = camera->getProjection();

        for(uint32_t i = 0; i < m_vInstances.size(); i++)
            ubo.instancePos[i] = m_vInstances.at(i);
        
        auto instanceCount = m_vInstances.size();
        m_pMesh->render(commandBuffer, imageIndex, ubo, instanceCount == 0 ? 1 : instanceCount);
    }
}

void CMeshComponentBase::cleanup()
{
    CComponentBase::cleanup();

    if (m_pMesh)
    {
        m_pMesh->cleanup();
    }
}

void CMeshComponentBase::destroy()
{
    CComponentBase::destroy();
}