#include "GLTFSceneNode.h"

using namespace Engine::Resources::Mesh;
using namespace Engine::Resources::Loaders;

void GLTFSceneNode::SetName(std::string srName)
{
    if (m_pParent)
    {
        auto pParent = shared_from_this();
        auto namePair = pParent->m_mUUID.find(m_srName);
        if (namePair != pParent->m_mUUID.end())
        {
            auto newPair = std::make_pair(srName, namePair->second);
            pParent->m_mUUID.erase(namePair);
            pParent->m_mUUID.emplace(newPair);
        }
    }
    m_srName = srName;
}

std::string &GLTFSceneNode::GetName()
{
    return m_srName;
}

std::string &GLTFSceneNode::GetUUID()
{
    return m_srUUID;
}

std::shared_ptr<GLTFSceneNode> &GLTFSceneNode::GetParent()
{
    return m_pParent;
}

std::map<std::string, std::shared_ptr<GLTFSceneNode>> &GLTFSceneNode::GetChilds()
{
    return m_mChilds;
}

// Deep search
std::shared_ptr<GLTFSceneNode> GLTFSceneNode::Find(std::string srName)
{
    auto it_id = m_mUUID.find(srName);
    if (it_id != m_mUUID.end())
    {
        auto it = m_mChilds.find(it_id->second);
        if (it != m_mChilds.end())
            return it->second;
    }
    for (auto &[name, child] : m_mChilds)
        child->Find(srName);
    return nullptr;
}

void GLTFSceneNode::AddChild(std::shared_ptr<GLTFSceneNode> child)
{
    m_mChilds.emplace(child->m_srUUID, child);
    m_mUUID.emplace(child->m_srName, child->m_srUUID);
    child->SetParent(shared_from_this());
}

void GLTFSceneNode::SetParent(std::shared_ptr<GLTFSceneNode> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}
void GLTFSceneNode::Attach(std::shared_ptr<GLTFSceneNode> child)
{
    AddChild(child);
}

void GLTFSceneNode::Detach(std::shared_ptr<GLTFSceneNode> child)
{
    auto it = m_mChilds.find(child->m_srUUID);
    if (it != m_mChilds.end())
    {
        auto it_id = m_mUUID.find(child->m_srName);
        if (it_id != m_mUUID.end())
            m_mUUID.erase(it_id);
        m_mChilds.erase(it);
    }
}

FTransform GLTFSceneNode::GetTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->GetTransform();
    return transform;
}

const glm::vec3 GLTFSceneNode::GetPosition() const
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->GetPosition();
    return position;
}

const glm::vec3 GLTFSceneNode::GetRotation() const
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->GetRotation();
    return rotation;
}

const glm::vec3 GLTFSceneNode::GetScale() const
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->GetScale();
    return scale;
}

void GLTFSceneNode::SetTransform(FTransform transformNew)
{
    m_transform = transformNew;
}

void GLTFSceneNode::SetPosition(glm::vec3 position)
{
    m_transform.pos = position;
}

void GLTFSceneNode::SetRotation(glm::vec3 rotation)
{
    m_transform.rot = rotation;
}

void GLTFSceneNode::SetScale(glm::vec3 scale)
{
    m_transform.scale = scale;
}