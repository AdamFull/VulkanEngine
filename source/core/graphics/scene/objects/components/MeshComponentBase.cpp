#include "MeshComponentBase.h"
#include "util/Frustum.hpp"
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
using namespace Engine::Util;

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
        auto camera = CCameraManager::inst()->getCurrentCamera();
        auto transform = getTransform();
        FUniformData ubo{};
        ubo.model = transform.getModel();
        ubo.view = camera->getView();
        ubo.projection = camera->getProjection();

        //Check frustum for instances
        uint32_t instanceCount{0};
        for(uint32_t i = 0; i < m_vInstances.size(); i++)
        {
            auto& pos = m_vInstances.at(i);
            if(true/*CFrustum::inst()->checkSphere(glm::vec4(getPosition(), 1.f) + pos, 1.f)*/)
            {
                ubo.instancePos[i] = pos;
                instanceCount++;
            }  
        }
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

void CMeshComponentBase::setInstances(const std::vector<glm::vec4> &vInstances)
{ 
    m_vInstances = vInstances;
    bHasInstances = !m_vInstances.empty(); 
}