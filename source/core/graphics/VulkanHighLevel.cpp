#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include <glslang/Public/ShaderLang.h>

using namespace Engine::Core;
using namespace Engine::Core::Window;

template<>
std::unique_ptr<VulkanHighLevel> utl::singleton<VulkanHighLevel>::_instance{nullptr};

VulkanHighLevel::~VulkanHighLevel()
{
    m_pDevice->GPUWait();
    UDevice->Destroy(m_pipelineCache);
}

void VulkanHighLevel::Create(FEngineCreateInfo createInfo)
{
    m_pWinHandle = std::make_shared<WindowHandle>();
    m_pDevice = std::make_shared<Device>();
    m_pSwapChain = std::make_shared<SwapChain>();
    m_pRenderer = std::make_shared<RenderSystem>();
    m_pVertexBufferObject = std::make_shared<VulkanVBO>();
    m_pOverlay = std::make_shared<ImguiOverlay>();

    m_pWinHandle->Create(createInfo.window);

    m_pDevice->Create(createInfo.device);

    CreatePipelineCache();

    m_pSwapChain->Create();

    m_pThreadPool = std::make_unique<utl::threadpool>();

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");
}

void VulkanHighLevel::CreatePipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    m_pipelineCache = UDevice->GetLogical().createPipelineCache(pipelineCacheCreateInfo);
}

void VulkanHighLevel::RecreateSwapChain()
{
    m_pWinHandle->Wait();
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pSwapChain->ReCreate();

    m_pRenderer->reCreate();
    m_pOverlay->ReCreate();
}

void VulkanHighLevel::CleanupSwapChain()
{
    m_pSwapChain->Cleanup();
    m_pRenderer->cleanup();
}

void VulkanHighLevel::Cleanup()
{
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pOverlay->Cleanup();
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