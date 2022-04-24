#include "CameraManager.h"

using namespace Engine::Core::Scene::Objects::Components;

template<>
std::unique_ptr<CameraManager> utl::singleton<CameraManager>::_instance{nullptr};

std::shared_ptr<CameraComponent> CameraManager::GetCurrentCamera()
{
    return m_vCameras.at(m_iCameraIndex);
}

void CameraManager::SetCurrentCameraIndex(size_t index)
{
    m_iCameraIndex = index;
}

void CameraManager::Attach(std::shared_ptr<CameraComponent> newCamera)
{
    m_vCameras.push_back(newCamera);
}
