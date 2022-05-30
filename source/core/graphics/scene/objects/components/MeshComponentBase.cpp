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

CMeshObjectBase::CMeshObjectBase()
{
    eObjectType = ERenderObjectType::eMesh;
}

void CMeshObjectBase::create()
{
    CRenderObject::create();

    if (m_pMesh)
        m_pMesh->create();
}

void CMeshObjectBase::reCreate()
{
    CRenderObject::reCreate();

    if (m_pMesh)
    {
        m_pMesh->reCreate();
    }
}

void CMeshObjectBase::update(float fDeltaTime)
{
    CRenderObject::update(fDeltaTime);

    if (m_pMesh)
    {

        
    }
}

void CMeshObjectBase::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    CRenderObject::render(commandBuffer, imageIndex);

    if (m_pMesh)
    {
        auto camera = CCameraManager::inst()->getCurrentCamera();
        auto transform = getTransform();
        auto& model = transform.getModel();

        //Check frustum for instances
        uint32_t instanceCount{0};
        for(uint32_t i = 0; i < m_vInstances.size(); i++)
        {
            auto& pos = m_vInstances.at(i);
            if(true/*CFrustum::inst()->checkSphere(glm::vec4(getPosition(), 1.f) + pos, 1.f)*/)
            {
                //ubo.instancePos[i] = pos;
                //instanceCount++;
            }  
        }
        m_pMesh->render(commandBuffer, model, instanceCount == 0 ? 1 : instanceCount);
    }
}

void CMeshObjectBase::cleanup()
{
    CRenderObject::cleanup();

    if (m_pMesh)
        m_pMesh->cleanup();
}

void CMeshObjectBase::destroy()
{
    CRenderObject::destroy();
}

void CMeshObjectBase::setInstances(const std::vector<glm::vec4> &vInstances)
{ 
    m_vInstances = vInstances;
    bHasInstances = !m_vInstances.empty(); 
}