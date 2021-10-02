#include "VulkanHighLevel.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUniform.h"
#include "VulkanBuffer.h"
#include "VulkanRenderer.h"
#include "Pipeline/PipelineManager.h"
#include "VulkanStaticHelper.h"
#include "WindowHandle.h"

namespace Engine
{
    std::unique_ptr<VulkanHighLevel> VulkanHighLevel::m_pInstance {nullptr};

    std::unique_ptr<VulkanHighLevel>& VulkanHighLevel::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new VulkanHighLevel());
        }
        return m_pInstance;
    }

    VulkanHighLevel::~VulkanHighLevel()
    {
        Destroy();
    }

    void VulkanHighLevel::Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion)
    {
        m_pDevice = std::make_unique<Device>();
        m_pSwapChain = std::make_unique<SwapChain>();
        m_pUniform = std::make_unique<UniformBuffer>();
        m_pRenderer = std::make_unique<Renderer>();

        m_pDevice->Create(pWindow, pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
        m_pSwapChain->Create(m_pDevice);
        m_pUniform->Create(m_pDevice, m_pSwapChain->GetImages().size());
        m_pRenderer->Create(m_pDevice, m_pSwapChain);
        UPipelineMGR->Create(m_pDevice, m_pSwapChain);
    }

    vk::CommandBuffer VulkanHighLevel::BeginFrame(bool* bResult)
    {
        vk::CommandBuffer commandBuffer;
        try { commandBuffer = m_pRenderer->BeginFrame(m_pDevice, m_pSwapChain); }
        catch (vk::OutOfDateKHRError err)
        {
            RecreateSwapChain();
            *bResult = false;
            return nullptr;
        }
        catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

        *bResult = true;
        return commandBuffer;
    }

    void VulkanHighLevel::EndFrame(vk::CommandBuffer commandBuffer, bool* bResult)
    {
        vk::Result resultPresent;
        try { resultPresent = m_pRenderer->EndFrame(m_pDevice, m_pSwapChain); }
        catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
        catch (vk::SystemError err) { throw std::runtime_error("failed to present swap chain image!"); }

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
        m_pDevice->GPUWait();

        CleanupSwapChain();
        m_pSwapChain->ReCreate(m_pDevice);
        m_pUniform->ReCreate(m_pDevice, m_pSwapChain->GetImages().size());

        m_pRenderer->ReCreate(m_pDevice, m_pSwapChain);
        UPipelineMGR->RecreatePipeline(m_pDevice, m_pSwapChain);
    }

    void VulkanHighLevel::CleanupSwapChain()
    {
        m_pSwapChain->Cleanup(m_pDevice);
        m_pUniform->Cleanup(m_pDevice);
        m_pRenderer->Cleanup(m_pDevice);
        UPipelineMGR->Cleanup(m_pDevice);
    }

    void VulkanHighLevel::Cleanup()
    {
        m_pDevice->GPUWait();

        CleanupSwapChain();
        m_pSwapChain->Destroy(m_pDevice);
        m_pDevice->Cleanup();
    }

    void VulkanHighLevel::Destroy()
    {
        m_pDevice->GPUWait();

        m_pSwapChain->Cleanup(m_pDevice);
        m_pUniform->Cleanup(m_pDevice);
        m_pRenderer->Destroy(m_pDevice);
        UPipelineMGR->Destroy(m_pDevice);
        m_pSwapChain->Destroy(m_pDevice);
        m_pDevice->Cleanup();
    }
}