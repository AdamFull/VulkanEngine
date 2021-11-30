#include "VulkanMesh.h"
#include "MeshFragment.h"

using namespace Engine::Core;
using namespace Engine::Resources::Mesh;

void MeshBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pUniformBuffer = std::make_shared<UniformBuffer>();
    m_pUniformBuffer->Create(2, sizeof(FUniformData));
    for (auto& fragment : m_vFragments)
        fragment->Create(pResMgr);
}

void MeshBase::ReCreate()
{
    m_pUniformBuffer->ReCreate(2);

    for (auto& fragment : m_vFragments)
        fragment->ReCreate();
}

void MeshBase::Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo)
{
    for (auto& fragment : m_vFragments)
    {
        //ubo.repeat = fRepeat;
        ubo.model = ubo.model * fragment->GetLocalMatrix();
        m_pUniformBuffer->UpdateUniformBuffer(imageIndex, &ubo);
        auto& buffer = m_pUniformBuffer->GetUniformBuffer(imageIndex);
        fragment->Update(buffer->GetDscriptor() ,imageIndex);
        fragment->Bind(commandBuffer, imageIndex);
    }
}

void MeshBase::Cleanup()
{
    m_pUniformBuffer->Cleanup();

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