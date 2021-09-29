#include "CameraManager.h"

namespace Engine
{
    std::unique_ptr<CameraManager> CameraManager::m_pInstance{nullptr};

    std::unique_ptr<CameraManager> &CameraManager::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new CameraManager());
        }
        return m_pInstance;
    }

    std::shared_ptr<Camera> CameraManager::GetCurrentCamera()
    {
        return m_vCameras.at(m_iCameraIndex);
    }

    void CameraManager::SetCurrentCameraIndex(size_t index)
    {
        m_iCameraIndex = index;
    }

    void CameraManager::Attach(std::shared_ptr<Camera> newCamera)
    {
        m_vCameras.emplace_back(newCamera);
    }
}