#pragma once

namespace Engine
{
    class CameraComponent;

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

        std::shared_ptr<CameraComponent> GetCurrentCamera();
        void SetCurrentCameraIndex(size_t index);
        void Attach(std::shared_ptr<CameraComponent> newCamera);
    private:
        std::vector<std::shared_ptr<CameraComponent>> m_vCameras;
        size_t m_iCameraIndex{0};
    };
}