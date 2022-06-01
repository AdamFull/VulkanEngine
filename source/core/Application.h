#pragma once
#include "keymapper/KeycodeConfig.h"
#include "controllers/CameraEditorController.h"

namespace engine
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
        scope_ptr<controllers::CCameraController> m_pCameraController;
    };
}