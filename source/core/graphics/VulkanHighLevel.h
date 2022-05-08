#pragma once
#include "util/threading.hpp"
#include "Window/WindowHandle.h"
#include "VulkanDevice.hpp"
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

#define UHLInstance Engine::Core::CVulkanHighLevel::inst()

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
            void create(FEngineCreateInfo createInfo);

            // TODO: Dont forget about clean textures
            void cleanup();
        };

        REGISTER_SERIALIZATION_BLOCK_H(FEngineCreateInfo);
    }
}
