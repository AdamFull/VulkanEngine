#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

template<>
std::unique_ptr<VulkanHighLevel> Singleton<VulkanHighLevel>::m_pInstance{nullptr};

VulkanHighLevel::~VulkanHighLevel()
{
    m_pDevice->GPUWait();
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

    m_pDevice->Create(createInfo.appName.c_str(), createInfo.appVersion, createInfo.engineName.c_str(), createInfo.engineVersion, createInfo.apiVersion);
    m_pSwapChain->Create();
    m_pRenderer->Create();
}

vk::CommandBuffer VulkanHighLevel::BeginFrame(bool *bResult)
{
    vk::CommandBuffer commandBuffer;
    try
    {
        commandBuffer = m_pRenderer->BeginFrame();
    }
    catch (vk::OutOfDateKHRError err)
    {
        RecreateSwapChain();
        *bResult = false;
        return nullptr;
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    *bResult = true;
    return commandBuffer;
}

void VulkanHighLevel::EndFrame(vk::CommandBuffer& commandBuffer, bool *bResult)
{
    vk::Result resultPresent;
    try
    {
        resultPresent = m_pRenderer->EndFrame();
    }
    catch (vk::OutOfDateKHRError err)
    {
        resultPresent = vk::Result::eErrorOutOfDateKHR;
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || WindowHandle::m_bWasResized)
    {
        WindowHandle::m_bWasResized = false;
        RecreateSwapChain();
        *bResult = false;
        return;
    }
}

void VulkanHighLevel::RecreateSwapChain()
{
    m_pWinHandle->Wait();
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pSwapChain->ReCreate();

    m_pRenderer->ReCreate();
    m_pOverlay->ReCreate();
}

void VulkanHighLevel::CleanupSwapChain()
{
    m_pSwapChain->Cleanup();
    m_pRenderer->Cleanup();
}

void VulkanHighLevel::Cleanup()
{
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pOverlay->Cleanup();
}