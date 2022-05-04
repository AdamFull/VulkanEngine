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
    CDevice::getInstance()->GPUWait();
    CDevice::getInstance()->destroy(m_pipelineCache);
}

void CVulkanHighLevel::create(FEngineCreateInfo createInfo)
{
    CWindowHandle::getInstance()->create(createInfo.window);
    CDevice::getInstance()->create(createInfo.device);

    createPipelineCache();

    CSwapChain::getInstance()->create();

    m_pThreadPool = std::make_unique<utl::threadpool>();

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");
}

void CVulkanHighLevel::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    m_pipelineCache = CDevice::getInstance()->getLogical().createPipelineCache(pipelineCacheCreateInfo);
}

void CVulkanHighLevel::recreateSwapChain()
{
    CWindowHandle::getInstance()->wait();
    CDevice::getInstance()->GPUWait();

    cleanupSwapChain();
    CSwapChain::getInstance()->reCreate();

    CRenderSystem::getInstance()->reCreate();
    CImguiOverlay::getInstance()->reCreate();
}

void CVulkanHighLevel::cleanupSwapChain()
{
    CSwapChain::getInstance()->cleanup();
    CRenderSystem::getInstance()->cleanup();
}

void CVulkanHighLevel::cleanup()
{
    CDevice::getInstance()->GPUWait();

    cleanupSwapChain();
    CImguiOverlay::getInstance()->cleanup();
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