#include "MeshNode.h"

using namespace Engine::Resources::Mesh;

void MeshNode::Create()
{

}

void MeshNode::ReCreate()
{

}

void MeshNode::Update(uint32_t imageIndex)
{

}

void MeshNode::Cleanup()
{

}

void MeshNode::Destroy()
{

}

void MeshNode::SetParent(std::shared_ptr<MeshNode> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}

void MeshNode::AddChild(std::shared_ptr<MeshNode> child)
{
    m_mChilds.emplace(child->GetName(), child);
    child->SetParent(shared_from_this());
}

void MeshNode::Attach(std::shared_ptr<MeshNode> child)
{
    AddChild(child);
}

void MeshNode::Detach(std::shared_ptr<MeshNode> child)
{
    auto it = m_mChilds.find(child->GetName());
    if (it != m_mChilds.end())
        m_mChilds.erase(it);
}

void MeshNode::SetTranslation(glm::vec3&& translation)
{
    m_vTranslation = translation;
}

void MeshNode::SetScale(glm::vec3&& scale)
{
    m_vScale = scale;
}

void MeshNode::SetRotation(glm::quat&& rotation)
{
    m_qRotation = rotation;
}

void MeshNode::SetLocalMatrix(glm::mat4&& matrix)
{
    m_mMatrix = matrix;
}

glm::mat4 MeshNode::GetLocalMatrix()
{
    return glm::translate(glm::mat4(1.0f), m_vTranslation) * glm::mat4(m_qRotation) * glm::scale(glm::mat4(1.0f), m_vScale) * m_mMatrix;
}

glm::mat4 MeshNode::GetMatrix()
{
    glm::mat4 matrix = GetLocalMatrix();
	std::shared_ptr<MeshNode> parent = GetParent();
	while (parent) 
    {
		matrix = parent->GetLocalMatrix() * matrix;
		parent = parent->GetParent();
	}
	return matrix;
}