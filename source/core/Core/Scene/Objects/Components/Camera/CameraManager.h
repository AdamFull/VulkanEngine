#pragma once
#include "Core/Scene/Objects/Components/Camera/CameraComponent.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                namespace Components
                {
                    class CameraManager : public utl::singleton<CameraManager>
                    {
                    public:
                        std::shared_ptr<CameraComponent> GetCurrentCamera();
                        void SetCurrentCameraIndex(size_t index);
                        void Attach(std::shared_ptr<CameraComponent> newCamera);

                    private:
                        std::vector<std::shared_ptr<CameraComponent>> m_vCameras;
                        size_t m_iCameraIndex{0};
                    };
                }
            }
        }
    }
}