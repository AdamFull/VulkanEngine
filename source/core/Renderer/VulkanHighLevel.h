#pragma once
#include "Window/WindowHandle.h"
#include "VulkanUniform.h"

#define UHLInstance Engine::VulkanHighLevel::GetInstance()
#define UWinHandle UHLInstance->GetWinHandle()
#define UDevice UHLInstance->GetDevice()
#define USwapChain UHLInstance->GetSwapChain()
#define UUniform UHLInstance->GetUniformBuffer()
#define UOverlay UHLInstance->GetOverlay()
#define URenderer UHLInstance->GetRenderer()
#define UVBO UHLInstance->GetVBO()

namespace Engine
{
    class Device;
    class SwapChain;
    class VulkanBuffer;
    class Renderer;
    class VulkanVBO;
    class ImguiOverlay;

    struct FEngineCreateInfo
    {
        FWindowCreateInfo window;
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

        VulkanHighLevel(const VulkanHighLevel&) = delete;
        void operator=(const VulkanHighLevel&) = delete;
        VulkanHighLevel(VulkanHighLevel&&) = delete;
        VulkanHighLevel& operator=(VulkanHighLevel&&) = delete;

        static std::unique_ptr<VulkanHighLevel>& GetInstance();

        void Create(FEngineCreateInfo createInfo);

        vk::CommandBuffer BeginFrame(bool* bResult);
        void EndFrame(vk::CommandBuffer commandBuffer, bool* bResult);

        void BeginRender(vk::CommandBuffer commandBuffer);
        void EndRender(vk::CommandBuffer commandBuffer);
        //TODO: Dont forget about clean textures
        void Cleanup();

        void Destroy();

        //Getters

        inline std::unique_ptr<WindowHandle>& GetWinHandle() { return m_pWinHandle; }
        inline std::unique_ptr<Device>& GetDevice() { return m_pDevice; }
        inline std::unique_ptr<SwapChain>& GetSwapChain() { return m_pSwapChain; }
        inline std::unique_ptr<UniformBuffer>&  GetUniformBuffer() { return m_pUniform; }
        inline std::unique_ptr<ImguiOverlay>&  GetOverlay() { return m_pOverlay; }
        inline std::unique_ptr<Renderer>&  GetRenderer() { return m_pRenderer; }
        inline std::unique_ptr<VulkanVBO>&  GetVBO() { return m_pVertexBufferObject; }
    private:
        void RecreateSwapChain();
        void CleanupSwapChain();

        //Window
        std::unique_ptr<WindowHandle> m_pWinHandle;
        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::unique_ptr<SwapChain> m_pSwapChain;

        std::unique_ptr<UniformBuffer> m_pUniform;

        std::unique_ptr<ImguiOverlay> m_pOverlay;

        std::unique_ptr<Renderer> m_pRenderer;

        std::unique_ptr<VulkanVBO> m_pVertexBufferObject;
    };
}
