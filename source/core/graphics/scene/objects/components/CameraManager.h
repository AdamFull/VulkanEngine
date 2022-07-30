#pragma once
#include "graphics/scene/objects/components/CameraComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CRenderObject;
            class CCameraManager
            {
            public:
                utl::weak_ptr<CRenderObject> &getCurrentCamera();
                void setCurrentCameraIndex(size_t index);
                void attach(utl::ref_ptr<CRenderObject>& newCamera);

            private:
                std::vector<utl::weak_ptr<CRenderObject>> vCameras;
                size_t iCameraIndex{0};
            };
        }
    }
}