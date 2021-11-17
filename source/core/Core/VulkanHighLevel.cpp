#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;

std::unique_ptr<VulkanHighLevel> VulkanHighLevel::m_pInstance{nullptr};

std::unique_ptr<VulkanHighLevel> &VulkanHighLevel::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance.reset(new VulkanHighLevel());
    }
    return m_pInstance;
}

VulkanHighLevel::~VulkanHighLevel()
{
    m_pDevice->GPUWait();

    m_pUniform->Cleanup();
    /*m_pRenderer->Destroy();
    m_pVertexBufferObject->Destroy();
    m_pOverlay->Destroy();
    m_pDevice->Cleanup();*/
}

void VulkanHighLevel::Create(FEngineCreateInfo createInfo)
{
    m_pWinHandle = std::make_shared<WindowHandle>();
    m_pDevice = std::make_shared<Device>(m_pWinHandle);
    m_pSwapChain = std::make_shared<SwapChain>(m_pDevice);
    m_pUniform = std::make_shared<UniformBuffer>(m_pDevice);
    m_pLightUniform = std::make_shared<UniformBuffer>(m_pDevice);
    m_pRenderer = std::make_shared<RenderSystem>(m_pDevice, m_pSwapChain);
    m_pVertexBufferObject = std::make_shared<VulkanVBO>(m_pDevice);
    m_pOverlay = std::make_shared<ImguiOverlay>(m_pWinHandle, m_pDevice, m_pSwapChain);

    m_pWinHandle->Create(createInfo.window);

    m_pDevice->Create(createInfo.appName.c_str(), createInfo.appVersion, createInfo.engineName.c_str(), createInfo.engineVersion, createInfo.apiVersion);
    m_pSwapChain->Create();
    m_pUniform->Create(m_pSwapChain->GetFramesInFlight(), sizeof(FUniformData));
    m_pLightUniform->Create(m_pSwapChain->GetFramesInFlight(), sizeof(FLightningData));
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
    //m_pUniform->ReCreate(m_pSwapChain->GetFramesInFlight());

    m_pRenderer->ReCreate();
    m_pOverlay->ReCreate();
}

void VulkanHighLevel::CleanupSwapChain()
{
    m_pSwapChain->Cleanup();
    //m_pUniform->Cleanup();
    m_pRenderer->Cleanup();
}

void VulkanHighLevel::Cleanup()
{
    m_pDevice->GPUWait();

    CleanupSwapChain();
    //m_pSwapChain->Destroy();
    m_pOverlay->Cleanup();
}