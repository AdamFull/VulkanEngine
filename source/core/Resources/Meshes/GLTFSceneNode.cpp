#include "GLTFSceneNode.h"

using namespace Engine::Objects;
using namespace Engine::Resources::Mesh;

void GLTFSceneNode::Create()
{
    for(auto& [name, node] : m_mChilds)
        node->Create();
}

void GLTFSceneNode::ReCreate()
{
    for(auto& [name, node] : m_mChilds)
        node->ReCreate();
}

void GLTFSceneNode::Update(uint32_t imageIndex)
{
    for(auto& [name, node] : m_mChilds)
        node->Update(imageIndex);
}

void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    
}

void GLTFSceneNode::Cleanup()
{
    for(auto& [name, node] : m_mChilds)
        node->Cleanup();
}

void GLTFSceneNode::Destroy()
{
    for(auto& [name, node] : m_mChilds)
        node->Destroy();
}

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
    m_mChilds.emplace(child->GetName(), child);
    child->SetParent(shared_from_this());
}

void GLTFSceneNode::Attach(std::shared_ptr<GLTFSceneNode> child)
{
    AddChild(child);
}

void GLTFSceneNode::Detach(std::shared_ptr<GLTFSceneNode> child)
{
    auto it = m_mChilds.find(child->GetName());
    if (it != m_mChilds.end())
        m_mChilds.erase(it);
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

void GLTFSceneNode::SetLocalMatrix(glm::mat4&& matrix)
{
    m_mMatrix = matrix;
}

glm::mat4 GLTFSceneNode::GetLocalMatrix()
{
    return m_transform.GetModel() * m_mMatrix;
}

glm::mat4 GLTFSceneNode::GetMatrix()
{
    glm::mat4 matrix = GetLocalMatrix();
	std::shared_ptr<GLTFSceneNode> parent = GetParent();
	while (parent) 
    {
		matrix = parent->GetLocalMatrix() * matrix;
		parent = parent->GetParent();
	}
	return matrix;
}