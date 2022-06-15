#include "CameraManager.h"

//TODO: move camera manager to another place
using namespace engine::core::scene;

template<>
scope_ptr<CCameraManager> utl::singleton<CCameraManager>::_instance{nullptr};

ref_ptr<CRenderObject>& CCameraManager::getCurrentCamera()
{
    return vCameras.at(iCameraIndex);
}

void CCameraManager::setCurrentCameraIndex(size_t index)
{
    iCameraIndex = index;
}

void CCameraManager::attach(ref_ptr<CRenderObject>& newCamera)
{
    vCameras.push_back(newCamera);
}
