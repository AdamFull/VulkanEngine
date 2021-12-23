#pragma once
#include "KeyMapping/KeycodeConfig.h"
#include "Controllers/CameraEditorController.h"
#include "RenderScene.h"

namespace Engine
{
    class Application
    {
    public:
        void Create();
        void ServiceHandle(EActionKey eKey, EKeyState eState);
        void run();
    private:
        std::unique_ptr<RenderScene> m_pRenderScene;
        std::unique_ptr<Controllers::CameraController> m_pCameraController;
    };
}