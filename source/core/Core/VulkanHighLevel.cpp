#include "VulkanHighLevel.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUniform.h"
#include "VulkanBuffer.h"
#include "VulkanRenderer.h"
#include "VulkanVBO.h"
#include "Overlay/ImguiOverlay.h"
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
    Destroy();
}

void VulkanHighLevel::Create(FEngineCreateInfo createInfo)
{
    m_pWinHandle = std::make_unique<WindowHandle>();
    m_pDevice = std::make_unique<Device>();
    m_pSwapChain = std::make_unique<SwapChain>();
    m_pUniform = std::make_unique<UniformBuffer>();
    m_pRenderer = std::make_unique<Renderer>();
    m_pVertexBufferObject = std::make_unique<VulkanVBO>();
    m_pOverlay = std::make_unique<ImguiOverlay>();

    m_pWinHandle->Create(createInfo.window);

    m_pDevice->Create(m_pWinHandle, createInfo.appName.c_str(), createInfo.appVersion, createInfo.engineName.c_str(), createInfo.engineVersion, createInfo.apiVersion);
    m_pSwapChain->Create(m_pDevice);
    m_pUniform->Create(m_pDevice, m_pSwapChain->GetFramesInFlight(), sizeof(FUniformData));
    m_pRenderer->Create(m_pDevice, m_pSwapChain);
    m_pOverlay->Create(m_pWinHandle, m_pDevice, m_pSwapChain);
}

vk::CommandBuffer VulkanHighLevel::BeginFrame(bool *bResult)
{
    vk::CommandBuffer commandBuffer;
    try
    {
        commandBuffer = m_pRenderer->BeginFrame(m_pDevice, m_pSwapChain);
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

void VulkanHighLevel::EndFrame(vk::CommandBuffer commandBuffer, bool *bResult)
{
    vk::Result resultPresent;
    try
    {
        resultPresent = m_pRenderer->EndFrame(m_pDevice, m_pSwapChain);
    }
    catch (vk::OutOfDateKHRError err)
    {
        resultPresent = vk::Result::eErrorOutOfDateKHR;
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR || WindowHandle::m_bWasResized)
    {
        WindowHandle::m_bWasResized = false;
        RecreateSwapChain();
        *bResult = false;
        return;
    }
}

void VulkanHighLevel::BeginRender(vk::CommandBuffer commandBuffer)
{
    m_pRenderer->BeginRender(commandBuffer, m_pSwapChain);
}

void VulkanHighLevel::EndRender(vk::CommandBuffer commandBuffer)
{
    m_pRenderer->EndRender(commandBuffer, m_pSwapChain);
}

void VulkanHighLevel::RecreateSwapChain()
{
    m_pWinHandle->Wait();
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pSwapChain->ReCreate(m_pDevice);
    m_pUniform->ReCreate(m_pDevice, m_pSwapChain->GetFramesInFlight());

    m_pRenderer->ReCreate(m_pDevice, m_pSwapChain);
    m_pOverlay->ReCreate(m_pDevice, m_pSwapChain);
}

void VulkanHighLevel::CleanupSwapChain()
{
    m_pSwapChain->Cleanup(m_pDevice);
    m_pUniform->Cleanup(m_pDevice);
    m_pRenderer->Cleanup(m_pDevice);
}

void VulkanHighLevel::Cleanup()
{
    m_pDevice->GPUWait();

    CleanupSwapChain();
    m_pSwapChain->Destroy(m_pDevice);
    m_pOverlay->Cleanup(m_pDevice);
    m_pDevice->Cleanup();
}

void VulkanHighLevel::Destroy()
{
    m_pDevice->GPUWait();

    m_pSwapChain->Cleanup(m_pDevice);
    m_pUniform->Cleanup(m_pDevice);
    m_pRenderer->Destroy(m_pDevice);
    m_pSwapChain->Destroy(m_pDevice);
    m_pVertexBufferObject->Destroy(m_pDevice);
    m_pOverlay->Destroy(m_pDevice);
    m_pDevice->Cleanup();
}