#pragma once

namespace Engine
{
    class CameraBase;

    class CameraManager
    {
    protected:
        CameraManager() = default;

        static std::unique_ptr<CameraManager> m_pInstance;
    public:
        CameraManager(const CameraManager&) = delete;
        void operator=(const CameraManager&) = delete;
        CameraManager(CameraManager&&) = delete;
        CameraManager& operator=(CameraManager&&) = delete;

        static std::unique_ptr<CameraManager>& GetInstance();

        std::shared_ptr<CameraBase> GetCurrentCamera();
        void SetCurrentCameraIndex(size_t index);
        void Attach(std::shared_ptr<CameraBase> newCamera);
    private:
        std::vector<std::shared_ptr<CameraBase>> m_vCameras;
        size_t m_iCameraIndex{0};
    };
}