#pragma once
#include "Window/WindowHandle.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanStaticHelper.h"
#include "VulkanBuffer.h"
#include "VulkanUniform.h"
#include "VulkanVBO.h"
#include "VulkanRenderSystem.h"
#include "Overlay/ImguiOverlay.h"

#include "Pipeline/VulkanPipeline.h"
#include "Pipeline/GraphicsPipeline.h"

#define UHLInstance Engine::Core::VulkanHighLevel::GetInstance()
#define UWinHandle UHLInstance->GetWinHandle()
#define UDevice UHLInstance->GetDevice()
#define USwapChain UHLInstance->GetSwapChain()
#define UOverlay UHLInstance->GetOverlay()
#define URenderer UHLInstance->GetRenderer()
#define UVBO UHLInstance->GetVBO()

namespace Engine
{
    namespace Core
    {
        struct FEngineCreateInfo
        {
            Window::FWindowCreateInfo window;
            std::string appName;
            uint32_t appVersion;
            std::string engineName;
            uint32_t engineVersion;
            uint32_t apiVersion;
        };

        class VulkanHighLevel
        {
        protected:
            VulkanHighLevel() = default;
            static std::unique_ptr<VulkanHighLevel> m_pInstance;

        public:
            ~VulkanHighLevel();

            VulkanHighLevel(const VulkanHighLevel &) = delete;
            void operator=(const VulkanHighLevel &) = delete;
            VulkanHighLevel(VulkanHighLevel &&) = delete;
            VulkanHighLevel &operator=(VulkanHighLevel &&) = delete;

            static std::unique_ptr<VulkanHighLevel> &GetInstance();

            void Create(FEngineCreateInfo createInfo);

            vk::CommandBuffer BeginFrame(bool *bResult);
            void EndFrame(vk::CommandBuffer& commandBuffer, bool *bResult);
            // TODO: Dont forget about clean textures
            void Cleanup();

            // Getters

            inline std::shared_ptr<Window::WindowHandle> GetWinHandle() { return m_pWinHandle; }
            inline std::shared_ptr<Device> GetDevice() { return m_pDevice; }
            inline std::shared_ptr<SwapChain> GetSwapChain() { return m_pSwapChain; }
            inline std::shared_ptr<ImguiOverlay> GetOverlay() { return m_pOverlay; }
            inline std::shared_ptr<RenderSystem> GetRenderer() { return m_pRenderer; }
            inline std::shared_ptr<VulkanVBO> GetVBO() { return m_pVertexBufferObject; }

        private:
            void RecreateSwapChain();
            void CleanupSwapChain();

            // Window
            std::shared_ptr<Window::WindowHandle> m_pWinHandle;
            // Device
            std::shared_ptr<Device> m_pDevice;
            // SwapChain
            std::shared_ptr<SwapChain> m_pSwapChain;

            std::shared_ptr<ImguiOverlay> m_pOverlay;

            std::shared_ptr<RenderSystem> m_pRenderer;

            std::shared_ptr<VulkanVBO> m_pVertexBufferObject;
        };
    }
}
