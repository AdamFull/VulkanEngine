#pragma once
#include "WindowHandle.h"
#include "InputMapper.h"
#include "Camera.h"
#include "CameraController.h"
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
        std::unique_ptr<InputMapper> m_pInputMapper;
        std::unique_ptr<CameraBase> m_pCamera;
        std::unique_ptr<CameraController> m_pCameraController;
        std::unique_ptr<VulkanHighLevel> m_pRender;

    };
}