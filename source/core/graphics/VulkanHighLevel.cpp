#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include <glslang/Public/ShaderLang.h>
#include "VulkanDevice.hpp"

using namespace engine::core;
using namespace engine::core::render;
using namespace engine::core::window;

template<>
utl::scope_ptr<CVulkanHighLevel> utl::singleton<CVulkanHighLevel>::_instance{nullptr};

//TODO: remove this class
void CVulkanHighLevel::create(FEngineCreateInfo createInfo)
{
    ci = createInfo;
    CWindowHandle::inst()->create(createInfo.window);
    CDevice::inst()->create(createInfo.device);

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");
}

void CVulkanHighLevel::cleanup()
{
    CDevice::inst()->GPUWait();
    CRenderSystem::inst()->cleanup();
    CVBO::inst()->cleanup();
    CDevice::inst()->cleanup();
}

namespace engine
{
    namespace core
    {
        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ELaunchMode,
            {
                {ELaunchMode::eGame, "game"},
                {ELaunchMode::eEditor, "editor"}
            }
        )
        void to_json(nlohmann::json &json, const FEngineModeCreateInfo &type)
        {
            json = nlohmann::json
            {
                {"mode", type.mode},
                {"start_scene", type.scene}
            };
        }

        void from_json(const nlohmann::json &json, FEngineModeCreateInfo &type)
        {
            ParseArgument(json, type.mode, "mode", true);
            ParseArgument(json, type.scene, "start_scene", true);
        }

        void to_json(nlohmann::json &json, const FEngineCreateInfo &type)
        {
            json = nlohmann::json
            {
                {"window", type.window},
                {"device", type.device},
                {"engine", type.engine}
            };
        }

        void from_json(const nlohmann::json &json, FEngineCreateInfo &type)
        {
            ParseArgument(json, type.window, "window", true);
            ParseArgument(json, type.device, "device", true);
            ParseArgument(json, type.engine, "engine", true);
        }
    }
}