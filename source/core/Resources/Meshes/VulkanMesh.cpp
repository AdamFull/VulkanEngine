#include "VulkanMesh.h"

using namespace Engine::Core;
using namespace Engine::Resources::Mesh;

void CMeshBase::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pUniformBuffer = std::make_shared<CUniformBuffer>();
    m_pUniformBuffer->create(2, sizeof(FUniformData));
    for (auto& fragment : m_vFragments)
        fragment->create(renderPass, subpass);
}

void CMeshBase::reCreate()
{
    m_pUniformBuffer->reCreate(2);

    for (auto& fragment : m_vFragments)
        fragment->reCreate();
}

void CMeshBase::render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo, uint32_t instanceCount)
{
    for (auto& fragment : m_vFragments)
    {
        //ubo.repeat = fRepeat;
        ubo.model = ubo.model * fragment->getLocalMatrix();
        ubo.normal = glm::transpose(glm::inverse(ubo.model));

        m_pUniformBuffer->updateUniformBuffer(imageIndex, &ubo);
        auto& buffer = m_pUniformBuffer->getUniformBuffer(imageIndex);
        auto descriptor = buffer->getDscriptor();
        fragment->update(descriptor ,imageIndex);
        fragment->bind(commandBuffer, imageIndex, instanceCount);
    }
}

void CMeshBase::cleanup()
{
    m_pUniformBuffer->cleanup();

    for (auto& fragment : m_vFragments)
        fragment->cleanup();
}

void CMeshBase::destroy()
{
    for (auto& fragment : m_vFragments)
        fragment->destroy();
}

void CMeshBase::addFragment(std::shared_ptr<CMeshFragment> fragment)
{
    m_vFragments.push_back(fragment);
}

void CMeshBase::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}