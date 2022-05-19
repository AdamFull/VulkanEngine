#include "VulkanMesh.h"
#include "util/uuid.hpp"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Core;
using namespace Engine::Resources::Mesh;

void CMeshBase::create()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();
    for (auto& fragment : m_vFragments)
        fragment->create();
}

void CMeshBase::reCreate()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    for (auto& fragment : m_vFragments)
        fragment->reCreate();
}

void CMeshBase::render(vk::CommandBuffer commandBuffer, glm::mat4& model, uint32_t instanceCount)
{
    for (auto& fragment : m_vFragments)
    {
        //ubo.repeat = fRepeat;
        auto mmodel = model * fragment->getLocalMatrix();

        fragment->render(commandBuffer, mmodel, instanceCount);
    }
}

void CMeshBase::cleanup()
{
    for (auto& fragment : m_vFragments)
        fragment->cleanup();
}

void CMeshBase::addFragment(std::shared_ptr<CMeshFragment> fragment)
{
    m_vFragments.push_back(fragment);
}

void CMeshBase::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}