#include "RenderObject.h"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Core::Scene;
using namespace Engine::Resources;

CRenderObject::CRenderObject()
{
}

CRenderObject::CRenderObject(std::string srName)
{
    m_srName = srName;
}

void CRenderObject::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    for (auto &[name, child] : m_mChilds)
        child->create(renderPass, subpass);
}

void CRenderObject::reCreate()
{
    for (auto &[name, child] : m_mChilds)
        child->reCreate();
}

void CRenderObject::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    for (auto &[name, child] : m_mChilds)
    {
        if (child->bVisible)
            child->render(commandBuffer, imageIndex);
    }
}

void CRenderObject::update(float fDeltaTime)
{
    for (auto &[name, child] : m_mChilds)
        child->update(fDeltaTime);
}

void CRenderObject::cleanup()
{
    for (auto &[name, child] : m_mChilds)
        child->cleanup();
}

void CRenderObject::destroy()
{
    for (auto &[name, child] : m_mChilds)
        child->destroy();
}

void CRenderObject::setName(std::string srName)
{
    m_srName = srName;
}

std::string &CRenderObject::getName()
{
    return m_srName;
}

std::shared_ptr<CRenderObject> &CRenderObject::getParent()
{
    return m_pParent;
}

std::unordered_map<std::string, std::shared_ptr<CRenderObject>> &CRenderObject::getChilds()
{
    return m_mChilds;
}

// Deep search
std::shared_ptr<CRenderObject> CRenderObject::find(std::string srName)
{
    auto it = m_mChilds.find(srName);
    if (it != m_mChilds.end())
        return it->second;

    for (auto &[name, child] : m_mChilds)
        child->find(srName);
    return nullptr;
}

void CRenderObject::addChild(std::shared_ptr<CRenderObject> child)
{
    m_mChilds.emplace(child->m_srName, child);
    child->setParent(shared_from_this());
}

void CRenderObject::setParent(std::shared_ptr<CRenderObject> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->detach(shared_from_this());
}
void CRenderObject::attach(std::shared_ptr<CRenderObject> child)
{
    addChild(child);
}

void CRenderObject::detach(std::shared_ptr<CRenderObject> child)
{
    auto it = m_mChilds.find(child->m_srName);
    if (it != m_mChilds.end())
    {
        m_mChilds.erase(it);
    }
}

FTransform CRenderObject::getTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->getTransform();
    return transform;
}

const glm::vec3 CRenderObject::getPosition() const
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->getPosition();
    return position;
}

const glm::vec3 CRenderObject::getRotation() const
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->getRotation();
    return rotation;
}

const glm::vec3 CRenderObject::getScale() const
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->getScale();
    return scale;
}

void CRenderObject::setTransform(FTransform transformNew)
{
    m_transform = transformNew;
}

void CRenderObject::setPosition(glm::vec3 position)
{
    m_transform.pos = position;
}

void CRenderObject::setRotation(glm::vec3 rotation)
{
    m_transform.rot = rotation;
}

void CRenderObject::setScale(glm::vec3 scale)
{
    m_transform.scale = scale;
}