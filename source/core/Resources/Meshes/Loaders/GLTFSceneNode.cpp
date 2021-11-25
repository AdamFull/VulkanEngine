#include "GLTFSceneNode.h"

using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources::Mesh;
using namespace Engine::Resources::Loaders;

void GLTFSceneNode::SetParent(std::shared_ptr<GLTFSceneNode> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}

void GLTFSceneNode::AddChild(std::shared_ptr<GLTFSceneNode> child)
{
    m_mChilds.emplace(child->m_srName, child);
    child->SetParent(shared_from_this());
}

void GLTFSceneNode::Attach(std::shared_ptr<GLTFSceneNode> child)
{
    AddChild(child);
}

void GLTFSceneNode::Detach(std::shared_ptr<GLTFSceneNode> child)
{
    auto it = m_mChilds.find(child->m_srName);
    if (it != m_mChilds.end())
        m_mChilds.erase(it);
}

std::shared_ptr<GLTFSceneNode> GLTFSceneNode::Find(uint32_t index)
{
    if(index == m_index)
        return shared_from_this();
    
    for(auto& [name, child] : m_mChilds)
        return Find(index);
    
    return nullptr;
}

FTransform GLTFSceneNode::GetTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->GetTransform();
    return transform;
}

glm::vec3 GLTFSceneNode::GetPosition()
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->GetPosition();
    return position;
}

glm::vec3 GLTFSceneNode::GetRotation()
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->GetRotation();
    return rotation;
}

glm::vec3 GLTFSceneNode::GetScale()
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->GetScale();
    return scale;
}