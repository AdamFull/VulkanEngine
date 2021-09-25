#pragma once
#include "WindowHandle.h"
#include "InputMapper.h"
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

        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWindow;
        std::unique_ptr<InputMapper> m_pInputMapper;
        std::unique_ptr<VulkanHighLevel> m_pRender;

    };
}