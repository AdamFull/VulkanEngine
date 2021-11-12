#include "VulkanMesh.h"
#include "MeshFragment.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Mesh;

void MeshBase::Create()
{
    for (auto& fragment : m_vFragments)
        fragment->Create();
}

void MeshBase::ReCreate()
{
    ResourceBase::ReCreate();
    for (auto& fragment : m_vFragments)
        fragment->ReCreate();
}

void MeshBase::Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo)
{
    for (auto& fragment : m_vFragments)
    {
        ubo.model = ubo.model * fragment->GetLocalMatrix();
        UUniform->UpdateUniformBuffer(imageIndex, &ubo);
        fragment->Update(imageIndex);
        fragment->Bind(commandBuffer, imageIndex);
    }
}

void MeshBase::Cleanup()
{
    ResourceBase::Cleanup();
    for (auto& fragment : m_vFragments)
        fragment->Cleanup();
}

void MeshBase::Destroy()
{
    ResourceBase::Destroy();
    for (auto& fragment : m_vFragments)
        fragment->Destroy();
}

void MeshBase::AddFragment(std::shared_ptr<MeshFragment> fragment)
{
    m_vFragments.push_back(fragment);
}