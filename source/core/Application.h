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
        void LoadTexture(std::string srPath);
        void LoadMesh(std::string srPath);

        void CreatePipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders);

        void ServiceHandle(EActionKey eKey);

        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWindow;
        std::unique_ptr<VulkanHighLevel> m_pRender;
        std::shared_ptr<SceneRootComponent> m_pRoot;

    };
}