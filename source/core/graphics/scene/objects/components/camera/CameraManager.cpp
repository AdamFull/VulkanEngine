#include "CameraManager.h"

using namespace Engine::Core::Scene;

template<>
std::unique_ptr<CCameraManager> utl::singleton<CCameraManager>::_instance{nullptr};

std::shared_ptr<CCameraComponent> CCameraManager::getCurrentCamera()
{
    return m_vCameras.at(m_iCameraIndex);
}

void CCameraManager::setCurrentCameraIndex(size_t index)
{
    m_iCameraIndex = index;
}

void CCameraManager::attach(std::shared_ptr<CCameraComponent> newCamera)
{
    m_vCameras.push_back(newCamera);
}
