#pragma once

#define UHLInstance Engine::VulkanHighLevel::GetInstance()
#define UDevice UHLInstance->GetDevice()
#define USwapChain UHLInstance->GetSwapChain()
#define UUniform UHLInstance->GetUniformBuffer()
#define URenderer UHLInstance->GetRenderer()

namespace Engine
{
    class WindowHandle;
    class Device;
    class SwapChain;
    class UniformBuffer;
    class VulkanBuffer;
    class Renderer;

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

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);


        vk::CommandBuffer BeginFrame(bool* bResult);
        void EndFrame(vk::CommandBuffer commandBuffer, bool* bResult);

        void BeginRender(vk::CommandBuffer commandBuffer);
        void EndRender(vk::CommandBuffer commandBuffer);
        //TODO: Dont forget about clean textures
        void Cleanup();

        void Destroy();

        //Getters

        inline std::unique_ptr<Device>& GetDevice() { return m_pDevice; }
        inline std::unique_ptr<SwapChain>& GetSwapChain() { return m_pSwapChain; }
        inline std::unique_ptr<UniformBuffer>&  GetUniformBuffer() { return m_pUniform; }
        inline std::unique_ptr<Renderer>&  GetRenderer() { return m_pRenderer; }
    private:
        void RecreateSwapChain();
        void CleanupSwapChain();

        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::unique_ptr<SwapChain> m_pSwapChain;

        //Descriptors
        std::unique_ptr<UniformBuffer> m_pUniform;

        std::unique_ptr<Renderer> m_pRenderer;
    };
}
