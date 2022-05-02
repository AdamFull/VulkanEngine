#pragma once
#include "Window/WindowHandle.h"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.h"
#include "VulkanStaticHelper.h"
#include "buffer/VulkanBuffer.h"
#include "buffer/VulkanUniform.h"
#include "buffer/VulkanVBO.h"
#include "VulkanRenderSystem.h"
#include "overlay/ImguiOverlay.h"
#include "serializer/Serialization.hpp"

#include "pipeline/Pipeline.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"

#define UHLInstance Engine::Core::CVulkanHighLevel::getInstance()
#define UWinHandle UHLInstance->getWinHandle()
#define UDevice UHLInstance->getDevice()
#define USwapChain UHLInstance->getSwapChain()
#define UOverlay UHLInstance->getOverlay()
#define URenderer UHLInstance->getRenderer()
#define UVBO UHLInstance->getVBO()
#define UThreadPool UHLInstance->getThreadPool()

namespace Engine
{
    namespace Core
    {
        struct FEngineCreateInfo
        {
            Window::FWindowCreateInfo window;
            FDeviceCreateInfo device;
        };

        class CVulkanHighLevel : public utl::singleton<CVulkanHighLevel>
        {
        public:
            ~CVulkanHighLevel();

            void create(FEngineCreateInfo createInfo);

            // TODO: Dont forget about clean textures
            void cleanup();

            // Getters

            inline std::shared_ptr<Window::CWindowHandle> getWinHandle() { return m_pWinHandle; }
            inline std::shared_ptr<CDevice> getDevice() { return m_pDevice; }
            inline std::shared_ptr<CSwapChain> getSwapChain() { return m_pSwapChain; }
            inline std::shared_ptr<CImguiOverlay> getOverlay() { return m_pOverlay; }
            inline std::shared_ptr<CRenderSystem> getRenderer() { return m_pRenderer; }
            inline std::shared_ptr<CVulkanVBO> getVBO() { return m_pVertexBufferObject; }
            inline std::unique_ptr<utl::threadpool>& getThreadPool() { return m_pThreadPool; }

            inline vk::PipelineCache& getPipelineCache() { return m_pipelineCache; }
            void recreateSwapChain();

        private:
            void createPipelineCache();
            void cleanupSwapChain();

            std::unique_ptr<utl::threadpool> m_pThreadPool;

            // Window
            std::shared_ptr<Window::CWindowHandle> m_pWinHandle;
            // Device
            std::shared_ptr<CDevice> m_pDevice;
            // SwapChain
            std::shared_ptr<CSwapChain> m_pSwapChain;
            std::shared_ptr<CImguiOverlay> m_pOverlay;
            std::shared_ptr<CRenderSystem> m_pRenderer;
            std::shared_ptr<CVulkanVBO> m_pVertexBufferObject;
            vk::PipelineCache m_pipelineCache;
        };

        REGISTER_SERIALIZATION_BLOCK_H(FEngineCreateInfo);
    }
}
