#include "RenderObject.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/scene/objects/components/CameraManager.h"

using namespace engine::core::scene;
using namespace engine::resources;

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

    if(pMesh) pMesh->create();
    if(pCamera && pCamera->getIsActive()) pCamera->create();
    if(pLight) pLight->create();
}

void CRenderObject::reCreate()
{
    for (auto &[name, child] : mChilds)
        child->reCreate();
    
    if(pMesh) pMesh->reCreate();
    if(pCamera && pCamera->getIsActive()) pCamera->reCreate();
    if(pLight) pLight->reCreate();
}

void CRenderObject::render(vk::CommandBuffer &commandBuffer)
{
    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    for (auto &[name, child] : mChilds)
    {
        if (child->isVisible())
            child->render(commandBuffer);
    }

    if(pMesh) pMesh->render(commandBuffer);
    if(pCamera && pCamera->getIsActive()) pCamera->render(commandBuffer);
    if(pLight) pLight->render(commandBuffer);
}

void CRenderObject::update(float fDeltaTime)
{
    for (auto &[name, child] : mChilds)
    {
        if(child->isEnabled())
            child->update(fDeltaTime);
    }

    if(pMesh) pMesh->update(fDeltaTime);
    if(pCamera && pCamera->getIsActive()) pCamera->update(fDeltaTime);
    if(pLight) pLight->update(fDeltaTime);
}

void CRenderObject::cleanup()
{
    for (auto &[name, child] : mChilds)
        child->cleanup();
    
    if(pMesh) pMesh->cleanup();
    if(pCamera && pCamera->getIsActive()) pCamera->cleanup();
    if(pLight) pLight->cleanup();
}

void CRenderObject::destroy()
{
    for (auto &[name, child] : mChilds)
        child->destroy();
    
    if(pMesh) pMesh->destroy();
    if(pCamera && pCamera->getIsActive()) pCamera->destroy();
    if(pLight) pLight->destroy();
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

utl::ref_ptr<CRenderObject> &CRenderObject::getParent()
{
    return pParent;
}

std::unordered_map<std::string, utl::ref_ptr<CRenderObject>> &CRenderObject::getChilds()
{
    return mChilds;
}

// Deep search
utl::ref_ptr<CRenderObject>& CRenderObject::find(std::string srName)
{
    auto it = mChilds.find(srName);
    if (it != mChilds.end())
        return it->second;

    for (auto &[name, child] : mChilds)
        return child->find(srName);
    return pNull;
}

utl::ref_ptr<CRenderObject>& CRenderObject::find(uint64_t id)
{
    for(auto& [name, child] : mChilds)
    {
        if(child->objectId == id)
            return child;
        return child->find(id);
    }
    return pNull;
}

void CRenderObject::addChild(utl::ref_ptr<CRenderObject>& child)
{
    mChilds.emplace(child->srName, child);
    child->setParent(shared_from_this());
}

void CRenderObject::setParent(utl::ref_ptr<CRenderObject> parent)
{
    pParentOld = pParent;
    pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (pParentOld)
        pParentOld->detach(shared_from_this());
}
void CRenderObject::attach(utl::ref_ptr<CRenderObject>&& child)
{
    addChild(child);
}

void CRenderObject::detach(utl::ref_ptr<CRenderObject> child)
{
    auto it = mChilds.find(child->srName);
    if (it != mChilds.end())
    {
        mChilds.erase(it);
    }
}

FTransform CRenderObject::getTransform()
{
    auto t = transform;
    if (pParent)
        t += pParent->getTransform();
    return t;
}

glm::mat4 CRenderObject::getModel()
{
    if (pParent)
        return pParent->getModel() * transform.getModel();
    return transform.getModel();
}

glm::vec3 CRenderObject::getPosition()
{
    FTransform global = this->getTransform();
    return global.getPosition();
}

glm::vec3 CRenderObject::getRotation()
{
    auto global =  this->getTransform();
    return global.getRotation();
}

glm::vec3 CRenderObject::getScale()
{
    auto global =  this->getTransform();
    return global.getScale();
}

void CRenderObject::setTransform(FTransform transformNew)
{
    transform = transformNew;
}

void CRenderObject::setPosition(glm::vec3 position)
{
    transform.setPosition(position);
}

void CRenderObject::setRotation(glm::vec3 rotation)
{
    transform.setRotation(rotation);
}

void CRenderObject::setScale(glm::vec3 scale)
{
    transform.setScale(scale);
}