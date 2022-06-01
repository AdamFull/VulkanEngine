#include "CameraManager.h"

//TODO: move camera manager to another place
using namespace engine::core::scene;

template<>
scope_ptr<CCameraManager> utl::singleton<CCameraManager>::_instance{nullptr};

ref_ptr<CCameraComponent>& CCameraManager::getCurrentCamera()
{
    return m_vCameras.at(m_iCameraIndex);
}

void CCameraManager::setCurrentCameraIndex(size_t index)
{
    m_iCameraIndex = index;
}

void CCameraManager::attach(ref_ptr<CCameraComponent> newCamera)
{
    m_vCameras.push_back(newCamera);
}
