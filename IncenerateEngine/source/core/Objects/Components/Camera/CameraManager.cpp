#include "CameraManager.h"
#include "CameraComponent.h"

using namespace Engine::Objects::Components;

std::unique_ptr<CameraManager> CameraManager::m_pInstance{nullptr};

std::unique_ptr<CameraManager> &CameraManager::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance.reset(new CameraManager());
    }
    return m_pInstance;
}

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
