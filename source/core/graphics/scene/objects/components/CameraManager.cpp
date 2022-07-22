#include "CameraManager.h"
#include "graphics/scene/objects/RenderObject.h"

//TODO: move camera manager to another place
using namespace engine::core::scene;

utl::ref_ptr<CRenderObject>& CCameraManager::getCurrentCamera()
{
    return vCameras.at(iCameraIndex);
}

void CCameraManager::setCurrentCameraIndex(size_t index)
{
    auto& oldCameraNode = vCameras.at(iCameraIndex);
    auto& oldCamera = oldCameraNode->getCamera();
    oldCamera->setActive(false);

    iCameraIndex = index;

    auto& cameraNode = vCameras.at(index);
    auto& camera = cameraNode->getCamera();
    camera->setActive(true);
}

void CCameraManager::attach(utl::ref_ptr<CRenderObject>& newCamera)
{
    if(vCameras.empty())
    {
        auto& camera = newCamera->getCamera();
        camera->setActive(true);
    }
        
    vCameras.push_back(newCamera);
}
