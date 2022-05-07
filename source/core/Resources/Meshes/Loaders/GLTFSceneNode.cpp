#include "GLTFSceneNode.h"

using namespace Engine::Resources::Mesh;
using namespace Engine::Resources::Loaders;

void GLTFSceneNode::setName(const std::string& srName)
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

std::string &GLTFSceneNode::getName()
{
    return m_srName;
}

std::string &GLTFSceneNode::getUUID()
{
    return m_srUUID;
}

std::shared_ptr<GLTFSceneNode> &GLTFSceneNode::getParent()
{
    return m_pParent;
}

std::map<std::string, std::shared_ptr<GLTFSceneNode>> &GLTFSceneNode::getChilds()
{
    return m_mChilds;
}

// Deep search
std::shared_ptr<GLTFSceneNode> GLTFSceneNode::find(const std::string& srName)
{
    auto it_id = m_mUUID.find(srName);
    if (it_id != m_mUUID.end())
    {
        auto it = m_mChilds.find(it_id->second);
        if (it != m_mChilds.end())
            return it->second;
    }
    for (auto &[name, child] : m_mChilds)
        child->find(srName);
    return nullptr;
}

void GLTFSceneNode::addChild(std::shared_ptr<GLTFSceneNode> child)
{
    m_mChilds.emplace(child->m_srUUID, child);
    m_mUUID.emplace(child->m_srName, child->m_srUUID);
    child->setParent(shared_from_this());
}

void GLTFSceneNode::setParent(std::shared_ptr<GLTFSceneNode> parent)
{
    m_pParentOld = m_pParent;
    m_pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (m_pParentOld)
        m_pParentOld->detach(shared_from_this());
}
void GLTFSceneNode::attach(std::shared_ptr<GLTFSceneNode> child)
{
    addChild(child);
}

void GLTFSceneNode::detach(std::shared_ptr<GLTFSceneNode> child)
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

FTransform GLTFSceneNode::getTransform()
{
    FTransform transform = m_transform;
    if (m_pParent)
        transform += m_pParent->getTransform();
    return transform;
}

const glm::vec3 GLTFSceneNode::getPosition() const
{
    glm::vec3 position = m_transform.pos;
    if (m_pParent)
        position += m_pParent->getPosition();
    return position;
}

const glm::vec3 GLTFSceneNode::getRotation() const
{
    glm::vec3 rotation = m_transform.rot;
    if (m_pParent)
        rotation += m_pParent->getRotation();
    return rotation;
}

const glm::vec3 GLTFSceneNode::getScale() const
{
    glm::vec3 scale = m_transform.scale;
    if (m_pParent)
        scale *= m_pParent->getScale();
    return scale;
}

void GLTFSceneNode::setTransform(FTransform transformNew)
{
    m_transform = transformNew;
}

void GLTFSceneNode::setPosition(glm::vec3 position)
{
    m_transform.pos = position;
}

void GLTFSceneNode::setRotation(glm::vec3 rotation)
{
    m_transform.rot = rotation;
}

void GLTFSceneNode::setScale(glm::vec3 scale)
{
    m_transform.scale = scale;
}