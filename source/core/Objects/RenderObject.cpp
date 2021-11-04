#include "RenderObject.h"
#include "Core/VulkanDevice.h"

using namespace Engine::Objects;
using namespace Engine::Resources;

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

std::shared_ptr<RenderObject> RenderObject::Find(std::string srName)
{
    auto it = m_mChilds.find(srName);
    if (it != m_mChilds.end())
        return it->second;

    for (auto &[name, child] : m_mChilds)
        child->Find(srName);

    return nullptr;
}

const glm::vec3 RenderObject::GetForwardVector()
{
    float yaw = m_transform.rot.y;
    float pitch = m_transform.rot.x;
    return glm::vec3{sin(yaw), -pitch, cos(yaw)};
}

const glm::vec3 RenderObject::GetRightVector()
{
    auto forward = GetForwardVector();
    return glm::vec3{forward.z, 0.f, -forward.x};
}

const glm::vec3 RenderObject::GetUpVector()
{
    return glm::vec3{0.f, -1.f, 0.f};
}

FTransform RenderObject::GetTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->GetTransform();
    return transform;
}

glm::vec3 RenderObject::GetPosition()
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->GetPosition();
    return position;
}

glm::vec3 RenderObject::GetRotation()
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->GetRotation();
    return rotation;
}

glm::vec3 RenderObject::GetScale()
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->GetScale();
    return scale;
}

void RenderObject::SetParent(std::shared_ptr<RenderObject> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->Detach(shared_from_this());
}

void RenderObject::AddChild(std::shared_ptr<RenderObject> child)
{
    m_mChilds.emplace(child->GetName(), child);
    child->SetParent(shared_from_this());
}

void RenderObject::Attach(std::shared_ptr<RenderObject> child)
{
    AddChild(child);
}

void RenderObject::Detach(std::shared_ptr<RenderObject> child)
{
    auto it = m_mChilds.find(child->GetName());
    if (it != m_mChilds.end())
        m_mChilds.erase(it);
}