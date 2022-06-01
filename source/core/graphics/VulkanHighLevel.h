#pragma once
#include "util/threading.hpp"
#include "Window/WindowHandle.h"
#include "VulkanDevice.hpp"
#include "VulkanStaticHelper.h"
#include "buffer/VulkanBuffer.h"
#include "GlobalVertexBuffer.h"
#include "VulkanRenderSystem.h"
#include "editor/EditorUI.h"
#include "serializer/Serialization.hpp"

#include "pipeline/Pipeline.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"
#include <util/helpers.hpp>

#define UHLInstance engine::core::CVulkanHighLevel::inst()

namespace engine
{
    namespace core
    {
        enum class ELaunchMode
        {
            eNone,
            eGame,
            eEditor
        };

        struct FEngineModeCreateInfo
        {
            ELaunchMode mode{ELaunchMode::eGame};
            std::string scene;
        };

        struct FEngineCreateInfo
        {
            window::FWindowCreateInfo window;
            FDeviceCreateInfo device;
            FEngineModeCreateInfo engine;
        };

        class CVulkanHighLevel : public utl::singleton<CVulkanHighLevel>
        {
        public:
            void create(FEngineCreateInfo createInfo);

            // TODO: Dont forget about clean textures
            void cleanup();

            const FEngineCreateInfo &getCI() const { return ci; }

        private:
            FEngineCreateInfo ci{};
        };

        REGISTER_SERIALIZATION_BLOCK_H(FEngineModeCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FEngineCreateInfo);
    }
}
