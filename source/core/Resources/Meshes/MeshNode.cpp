#include "MeshNode.h"

using namespace Engine::Resources::Mesh;

void MeshNode::Create()
{
    ResourceBase::Create();
}

void MeshNode::ReCreate()
{
    ResourceBase::ReCreate();
}

void MeshNode::Update(uint32_t imageIndex)
{
    ResourceBase::Update(imageIndex);
}

void MeshNode::Cleanup()
{
    ResourceBase::Cleanup();
}

void MeshNode::Destroy()
{
    ResourceBase::Destroy();
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