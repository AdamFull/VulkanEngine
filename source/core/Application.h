#pragma once
#include "keymapper/KeycodeConfig.h"
#include "controllers/CameraEditorController.h"
#include "RenderScene.h"

namespace Engine
{
    class CApplication
    {
    public:
        void create();
        void serviceHandle(EActionKey eKey, EKeyState eState);
        void run();
    private:
        std::unique_ptr<CRenderScene> m_pRenderScene;
        std::unique_ptr<Controllers::CCameraController> m_pCameraController;
    };
}