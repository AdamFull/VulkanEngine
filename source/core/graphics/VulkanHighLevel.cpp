#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include <glslang/Public/ShaderLang.h>

using namespace Engine::Core;
using namespace Engine::Core::Window;

template<>
std::unique_ptr<CVulkanHighLevel> utl::singleton<CVulkanHighLevel>::_instance{nullptr};

CVulkanHighLevel::~CVulkanHighLevel()
{
    m_pDevice->GPUWait();
    UDevice->destroy(m_pipelineCache);
}

void CVulkanHighLevel::create(FEngineCreateInfo createInfo)
{
    m_pWinHandle = std::make_shared<CWindowHandle>();
    m_pDevice = std::make_shared<CDevice>();
    m_pSwapChain = std::make_shared<CSwapChain>();
    m_pRenderer = std::make_shared<CRenderSystem>();
    m_pVertexBufferObject = std::make_shared<CVulkanVBO>();
    m_pOverlay = std::make_shared<CImguiOverlay>();

    m_pWinHandle->create(createInfo.window);

    m_pDevice->create(createInfo.device);

    createPipelineCache();

    m_pSwapChain->create();

    m_pThreadPool = std::make_unique<utl::threadpool>();

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");
}

void CVulkanHighLevel::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    m_pipelineCache = UDevice->getLogical().createPipelineCache(pipelineCacheCreateInfo);
}

void CVulkanHighLevel::recreateSwapChain()
{
    m_pWinHandle->wait();
    m_pDevice->GPUWait();

    cleanupSwapChain();
    m_pSwapChain->reCreate();

    m_pRenderer->reCreate();
    m_pOverlay->reCreate();
}

void CVulkanHighLevel::cleanupSwapChain()
{
    m_pSwapChain->cleanup();
    m_pRenderer->cleanup();
}

void CVulkanHighLevel::cleanup()
{
    m_pDevice->GPUWait();

    cleanupSwapChain();
    m_pOverlay->cleanup();
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