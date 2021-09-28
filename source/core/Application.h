#pragma once
#include "WindowHandle.h"
#include "KeycodeConfig.h"
#include "Objects/Components/SceneRootComponent.h"
#include "VulkanHighLevel.h"

namespace Engine
{
    class Application
    {
    public:
        void Create();
        void LoadMesh(std::string srPath);

        void ServiceHandle(EActionKey eKey);

        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWindow;
        std::unique_ptr<VulkanHighLevel> m_pRender;
    };
}