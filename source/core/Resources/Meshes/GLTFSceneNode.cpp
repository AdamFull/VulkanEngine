#include "GLTFSceneNode.h"

using namespace Engine::Resources::Mesh;

void GLTFSceneNode::Create()
{

}

void GLTFSceneNode::ReCreate()
{

}

void GLTFSceneNode::Update(uint32_t imageIndex)
{

}

void GLTFSceneNode::Cleanup()
{

}

void GLTFSceneNode::Destroy()
{

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

void GLTFSceneNode::SetTranslation(glm::vec3&& translation)
{
    m_vTranslation = translation;
}

void GLTFSceneNode::SetScale(glm::vec3&& scale)
{
    m_vScale = scale;
}

void GLTFSceneNode::SetRotation(glm::quat&& rotation)
{
    m_qRotation = rotation;
}

void GLTFSceneNode::SetLocalMatrix(glm::mat4&& matrix)
{
    m_mMatrix = matrix;
}

glm::mat4 GLTFSceneNode::GetLocalMatrix()
{
    return glm::translate(glm::mat4(1.0f), m_vTranslation) * glm::mat4(m_qRotation) * glm::scale(glm::mat4(1.0f), m_vScale) * m_mMatrix;
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