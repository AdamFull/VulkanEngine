#include "ResourceBase.h"

using namespace Engine::Resources;
void ResourceBase::Create()
{
    for(auto& [name, child] : m_mChilds)
        child->Create();
}

void ResourceBase::ReCreate()
{
    for(auto& [name, child] : m_mChilds)
        child->ReCreate();
}

void ResourceBase::Update(uint32_t imageIndex)
{
    for(auto& [name, child] : m_mChilds)
        child->Update(imageIndex);
}

void ResourceBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    for(auto& [name, child] : m_mChilds)
        child->Bind(commandBuffer, imageIndex);
}

void ResourceBase::Cleanup()
{
    for(auto& [name, child] : m_mChilds)
        child->Create();
}

void ResourceBase::Destroy()
{
    for(auto& [name, child] : m_mChilds)
        child->Destroy();
}

void ResourceBase::SetParent(std::shared_ptr<ResourceBase> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}

void ResourceBase::AddChild(std::shared_ptr<ResourceBase> child)
{
    m_mChilds.emplace(child->GetName(), child);
    child->SetParent(shared_from_this());
}

void ResourceBase::Attach(std::shared_ptr<ResourceBase> child)
{
    AddChild(child);
}

void ResourceBase::Detach(std::shared_ptr<ResourceBase> child)
{
    auto it = m_mChilds.find(child->GetName());
    if (it != m_mChilds.end())
        m_mChilds.erase(it);
}