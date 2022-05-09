#include "RenderObject.h"
#include "util/Frustum.hpp"
#include "graphics/VulkanDevice.hpp"

using namespace Engine::Util;
using namespace Engine::Core::Scene;
using namespace Engine::Resources;

static uint64_t _objectIdCounter{0};

CRenderObject::CRenderObject()
{
    objectId = _objectIdCounter;
    _objectIdCounter++;
}

CRenderObject::CRenderObject(std::string name)
{
    srName = name;
}

CRenderObject::~CRenderObject()
{
    mChilds.clear();
}

void CRenderObject::create()
{
    for (auto &[name, child] : mChilds)
        child->create();
}

void CRenderObject::reCreate()
{
    for (auto &[name, child] : mChilds)
        child->reCreate();
}

void CRenderObject::render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    if (bEnable)
    {
        for (auto &[name, child] : mChilds)
        {
            bool needToRender = child->isVisible();
            if (child->isCullable())
            {
                switch (child->getCullingType())
                {
                case ECullingType::eByPoint:
                {
                    needToRender = needToRender && CFrustum::inst()->checkPoint(child->getPosition());
                }
                break;
                case ECullingType::eBySphere:
                {
                    needToRender = needToRender && CFrustum::inst()->checkSphere(child->getPosition(), child->getCullingRadius());
                }
                break;
                case ECullingType::eByBox:
                {
                    auto &bounds = child->getBounds();
                    needToRender = needToRender && CFrustum::inst()->checkBox(child->getPosition() + bounds.first, child->getPosition() + bounds.second);
                }
                break;
                }
            }

            if (needToRender)
            {
                child->render(commandBuffer, imageIndex);
            }
        }
    }
}

void CRenderObject::update(float fDeltaTime)
{
    if (bEnable)
    {
        for (auto &[name, child] : mChilds)
            child->update(fDeltaTime);
    }
}

void CRenderObject::cleanup()
{
    for (auto &[name, child] : mChilds)
        child->cleanup();
}

void CRenderObject::destroy()
{
    for (auto &[name, child] : mChilds)
        child->destroy();
}

void CRenderObject::setName(std::string name)
{
    if(name == srName)
        return;
        
    if(pParent)
    {
        auto& childs = pParent->getChilds();
        auto child = childs.extract(srName);
        if(child)
        {
            child.key() = name;
            childs.insert(std::move(child));
        }
    }
    srName = name;
}

std::string &CRenderObject::getName()
{
    return srName;
}

std::shared_ptr<CRenderObject> &CRenderObject::getParent()
{
    return pParent;
}

std::unordered_map<std::string, std::shared_ptr<CRenderObject>> &CRenderObject::getChilds()
{
    return mChilds;
}

// Deep search
std::shared_ptr<CRenderObject> CRenderObject::find(std::string srName)
{
    auto it = mChilds.find(srName);
    if (it != mChilds.end())
        return it->second;

    for (auto &[name, child] : mChilds)
        child->find(srName);
    return nullptr;
}

std::shared_ptr<CRenderObject> CRenderObject::find(uint64_t id)
{
    for(auto& [name, child] : mChilds)
    {
        if(child->objectId == id)
            return child;
        return child->find(id);
    }
    return nullptr;
}

void CRenderObject::addChild(std::shared_ptr<CRenderObject> child)
{
    mChilds.emplace(child->srName, child);
    child->setParent(shared_from_this());
}

void CRenderObject::setParent(std::shared_ptr<CRenderObject> parent)
{
    pParentOld = pParent;
    pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (pParentOld)
        pParentOld->detach(shared_from_this());
}
void CRenderObject::attach(std::shared_ptr<CRenderObject> child)
{
    addChild(child);
}

void CRenderObject::detach(std::shared_ptr<CRenderObject> child)
{
    auto it = mChilds.find(child->srName);
    if (it != mChilds.end())
    {
        mChilds.erase(it);
    }
}

FTransform CRenderObject::getTransform()
{
    FTransform t = transform;
    if (pParent)
        t += pParent->getTransform();
    return t;
}

const glm::vec3 CRenderObject::getPosition() const
{
    glm::vec3 position = transform.pos;
    if (pParent)
        position += pParent->getPosition();
    return position;
}

const glm::vec3 CRenderObject::getRotation() const
{
    glm::vec3 rotation = transform.rot;
    if (pParent)
        rotation += pParent->getRotation();
    return rotation;
}

const glm::vec3 CRenderObject::getScale() const
{
    glm::vec3 scale = transform.scale;
    if (pParent)
        scale *= pParent->getScale();
    return scale;
}

void CRenderObject::setTransform(FTransform transformNew)
{
    transform = transformNew;
}

void CRenderObject::setPosition(glm::vec3 position)
{
    transform.pos = position;
}

void CRenderObject::setRotation(glm::vec3 rotation)
{
    transform.rot = rotation;
}

void CRenderObject::setScale(glm::vec3 scale)
{
    transform.scale = scale;
}