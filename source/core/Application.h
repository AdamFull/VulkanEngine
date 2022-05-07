#pragma once
#include "keymapper/KeycodeConfig.h"
#include "controllers/CameraEditorController.h"

namespace Engine
{
    /**
     * @brief Main application class
     * 
     */
    class CApplication
    {
    public:
        /**
         * @brief Creates application and begins engine initialization
         * 
         */
        void create();

        /**
         * @brief Service function to handle key input
         * 
         * @param eKey Key type
         * @param eState Key state
         */
        void serviceHandle(EActionKey eKey, EKeyState eState);

        /**
         * @brief Starts main application loop
         * 
         */
        void run();
    private:
        std::unique_ptr<Controllers::CCameraController> m_pCameraController;
    };
}