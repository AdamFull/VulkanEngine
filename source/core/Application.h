#pragma once
#include "KeyMapping/KeycodeConfig.h"
#include "Camera/Controllers/CameraEditorController.h"
#include "RenderScene.h"

namespace Engine
{
    class Application
    {
    public:
        void Create();
        void ServiceHandle(EActionKey eKey);
        void run();
    private:
        std::unique_ptr<RenderScene> m_pRenderScene;
        std::unique_ptr<CameraController> m_pCameraController;
    };
}