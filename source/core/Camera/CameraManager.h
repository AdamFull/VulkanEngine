#pragma once

namespace Engine
{
    class Camera;

    class CameraManager
    {
    protected:
        CameraManager();

        static std::unique_ptr<CameraManager> m_pInstance;
    public:
        CameraManager(const CameraManager&) = delete;
        void operator=(const CameraManager&) = delete;
        CameraManager(CameraManager&&) = delete;
        CameraManager& operator=(CameraManager&&) = delete;

        std::unique_ptr<CameraManager>& GetInstance();

        std::shared_ptr<Camera> GetCurrentCamera();
        void SetCurrentCameraIndex(size_t index);
        void Attach(std::shared_ptr<Camera> newCamera);
    private:
        std::vector<std::shared_ptr<Camera>> m_vCameras;
        size_t m_iCameraIndex{0};
    };
}