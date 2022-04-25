#include "RenderObject.h"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;

RenderObject::RenderObject()
{
    //m_srUUID = uuid::generate();
}

RenderObject::RenderObject(std::string srName)
{
    m_srName = srName;
    //m_srUUID = uuid::generate();
}

void RenderObject::Create(std::shared_ptr<ResourceManager> resourceMgr)
{
    for (auto &[name, child] : m_mChilds)
        child->Create(resourceMgr);
}

void RenderObject::ReCreate()
{
    for (auto &[name, child] : m_mChilds)
        child->ReCreate();
}

void RenderObject::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    for (auto &[name, child] : m_mChilds)
        child->Render(commandBuffer, imageIndex);
}

void RenderObject::Update(float fDeltaTime)
{
    for (auto &[name, child] : m_mChilds)
        child->Update(fDeltaTime);
}

void RenderObject::Cleanup()
{
    for (auto &[name, child] : m_mChilds)
        child->Cleanup();
}

void RenderObject::Destroy()
{
    for (auto &[name, child] : m_mChilds)
        child->Destroy();
}

void RenderObject::SetName(std::string srName)
{
    /*if (m_pParent)
    {
        auto pParent = shared_from_this();
        auto namePair = pParent->m_mUUID.find(m_srName);
        if (namePair != pParent->m_mUUID.end())
        {
            auto newPair = std::make_pair(srName, namePair->second);
            pParent->m_mUUID.erase(namePair);
            pParent->m_mUUID.emplace(newPair);
        }
    }*/
    m_srName = srName;
}

std::string &RenderObject::GetName()
{
    return m_srName;
}

/*std::string &RenderObject::GetUUID()
{
    return m_srUUID;
}*/

std::shared_ptr<RenderObject> &RenderObject::GetParent()
{
    return m_pParent;
}

std::unordered_map<std::string, std::shared_ptr<RenderObject>> &RenderObject::GetChilds()
{
    return m_mChilds;
}

// Deep search
std::shared_ptr<RenderObject> RenderObject::Find(std::string srName)
{
    /*auto it_id = m_mUUID.find(srName);
    if (it_id != m_mUUID.end())
    {
        auto it = m_mChilds.find(it_id->second);
        if (it != m_mChilds.end())
            return it->second;
    }*/
    auto it = m_mChilds.find(srName);
    if (it != m_mChilds.end())
        return it->second;

    for (auto &[name, child] : m_mChilds)
        child->Find(srName);
    return nullptr;
}

void RenderObject::AddChild(std::shared_ptr<RenderObject> child)
{
    m_mChilds.emplace(child->m_srName, child);
    //m_mUUID.emplace(child->m_srName, child->m_srUUID);
    child->SetParent(shared_from_this());
}

void RenderObject::SetParent(std::shared_ptr<RenderObject> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}
void RenderObject::Attach(std::shared_ptr<RenderObject> child)
{
    AddChild(child);
}

void RenderObject::Detach(std::shared_ptr<RenderObject> child)
{
    /*auto it = m_mChilds.find(child->m_srUUID);
    if (it != m_mChilds.end())
    {
        auto it_id = m_mUUID.find(child->m_srName);
        if (it_id != m_mUUID.end())
            m_mUUID.erase(it_id);
        m_mChilds.erase(it);
    }*/
    auto it = m_mChilds.find(child->m_srName);
    if (it != m_mChilds.end())
    {
        m_mChilds.erase(it);
    }
}

FTransform RenderObject::GetTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->GetTransform();
    return transform;
}

const glm::vec3 RenderObject::GetPosition() const
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->GetPosition();
    return position;
}

const glm::vec3 RenderObject::GetRotation() const
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->GetRotation();
    return rotation;
}

const glm::vec3 RenderObject::GetScale() const
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->GetScale();
    return scale;
}

void RenderObject::SetTransform(FTransform transformNew)
{
    m_transform = transformNew;
}

void RenderObject::SetPosition(glm::vec3 position)
{
    m_transform.pos = position;
}

void RenderObject::SetRotation(glm::vec3 rotation)
{
    m_transform.rot = rotation;
}

void RenderObject::SetScale(glm::vec3 scale)
{
    m_transform.scale = scale;
}