#include "CameraManager.h"
#include "graphics/scene/objects/RenderObject.h"

//TODO: move camera manager to another place
using namespace engine::core::scene;

utl::weak_ptr<CRenderObject>& CCameraManager::getCurrentCamera()
{
    return vCameras.at(iCameraIndex);
}

void CCameraManager::setCurrentCameraIndex(size_t index)
{
    auto& oldCameraNode = vCameras.at(iCameraIndex);
    auto oldCamera = oldCameraNode.lock()->getComponent<CCameraComponent>().lock();
    oldCamera->setActive(false);

    iCameraIndex = index;

    auto& cameraNode = vCameras.at(index);
    auto camera = cameraNode.lock()->getComponent<CCameraComponent>().lock();
    camera->setActive(true);
}

void CCameraManager::attach(utl::ref_ptr<CRenderObject>& newCamera)
{
    if(vCameras.empty())
    {
        auto camera = newCamera->getComponent<CCameraComponent>().lock();
        camera->setActive(true);
    }
        
    vCameras.push_back(newCamera);
}
