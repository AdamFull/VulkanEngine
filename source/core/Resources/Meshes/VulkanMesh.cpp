#include "VulkanMesh.h"

using namespace Engine::Core;
using namespace Engine::Resources::Mesh;

void MeshBase::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    m_pUniformBuffer = std::make_shared<CUniformBuffer>();
    m_pUniformBuffer->create(2, sizeof(FUniformData));
    for (auto& fragment : m_vFragments)
        fragment->Create(renderPass, subpass);
}

void MeshBase::ReCreate()
{
    m_pUniformBuffer->reCreate(2);

    for (auto& fragment : m_vFragments)
        fragment->ReCreate();
}

void MeshBase::Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo, uint32_t instanceCount)
{
    for (auto& fragment : m_vFragments)
    {
        //ubo.repeat = fRepeat;
        ubo.model = ubo.model * fragment->GetLocalMatrix();
        ubo.normal = glm::transpose(glm::inverse(ubo.model));

        m_pUniformBuffer->updateUniformBuffer(imageIndex, &ubo);
        auto& buffer = m_pUniformBuffer->getUniformBuffer(imageIndex);
        auto descriptor = buffer->GetDscriptor();
        fragment->Update(descriptor ,imageIndex);
        fragment->Bind(commandBuffer, imageIndex, instanceCount);
    }
}

void MeshBase::Cleanup()
{
    m_pUniformBuffer->cleanup();

    for (auto& fragment : m_vFragments)
        fragment->Cleanup();
}

void MeshBase::Destroy()
{
    for (auto& fragment : m_vFragments)
        fragment->Destroy();
}

void MeshBase::AddFragment(std::shared_ptr<MeshFragment> fragment)
{
    m_vFragments.push_back(fragment);
}

void MeshBase::SetName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}