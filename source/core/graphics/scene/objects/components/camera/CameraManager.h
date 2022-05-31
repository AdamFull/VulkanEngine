#pragma once
#include "graphics/scene/objects/components/camera/CameraComponent.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CCameraManager : public utl::singleton<CCameraManager>
            {
            public:
                ref_ptr<CCameraComponent>& getCurrentCamera();
                void setCurrentCameraIndex(size_t index);
                void attach(ref_ptr<CCameraComponent> newCamera);

            private:
                std::vector<ref_ptr<CCameraComponent>> m_vCameras;
                size_t m_iCameraIndex{0};
            };
        }
    }
}