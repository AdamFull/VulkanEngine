#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include <glslang/Public/ShaderLang.h>
#include "VulkanDevice.hpp"

using namespace Engine::Core;
using namespace Engine::Core::Window;

template<>
std::unique_ptr<CVulkanHighLevel> utl::singleton<CVulkanHighLevel>::_instance{nullptr};

CVulkanHighLevel::~CVulkanHighLevel()
{
    CDevice::inst()->GPUWait();
    CDevice::inst()->destroy(m_pipelineCache);
}

void CVulkanHighLevel::create(FEngineCreateInfo createInfo)
{
    CWindowHandle::inst()->create(createInfo.window);
    CDevice::inst()->create(createInfo.device);
    createPipelineCache();
    CSwapChain::inst()->create();

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");
}

void CVulkanHighLevel::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    m_pipelineCache = CDevice::inst()->getLogical().createPipelineCache(pipelineCacheCreateInfo);
}

void CVulkanHighLevel::recreateSwapChain()
{
    CWindowHandle::inst()->wait();
    CDevice::inst()->GPUWait();

    cleanupSwapChain();
    CSwapChain::inst()->reCreate();

    CRenderSystem::inst()->reCreate();
}

void CVulkanHighLevel::cleanupSwapChain()
{
    CSwapChain::inst()->cleanup();
    CRenderSystem::inst()->cleanup();
}

void CVulkanHighLevel::cleanup()
{
    
}

namespace Engine
{
    namespace Core
    {
        void to_json(nlohmann::json &json, const FEngineCreateInfo &type)
        {
            json = nlohmann::json{
                {"window", type.window},
                {"device", type.device}
                };
        }

        void from_json(const nlohmann::json &json, FEngineCreateInfo &type)
        {
            ParseArgument(json, type.window, "window", true);
            ParseArgument(json, type.device, "device", true);
        }
    }
}