#pragma once
#include "util/threading.hpp"
#include "Window/WindowHandle.h"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.h"
#include "VulkanStaticHelper.h"
#include "buffer/VulkanBuffer.h"
#include "buffer/VulkanUniform.h"
#include "GlobalVertexBuffer.h"
#include "VulkanRenderSystem.h"
#include "overlay/ImguiOverlay.h"
#include "serializer/Serialization.hpp"

#include "pipeline/Pipeline.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"
#include <util/helpers.hpp>

#define UHLInstance Engine::Core::CVulkanHighLevel::getInstance()
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
            inline std::unique_ptr<utl::threadpool>& getThreadPool() { return m_pThreadPool; }

            inline vk::PipelineCache& getPipelineCache() { return m_pipelineCache; }
            void recreateSwapChain();

        private:
            void createPipelineCache();
            void cleanupSwapChain();

            std::unique_ptr<utl::threadpool> m_pThreadPool;
            vk::PipelineCache m_pipelineCache;
        };

        REGISTER_SERIALIZATION_BLOCK_H(FEngineCreateInfo);
    }
}
