#include "PipelineManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "VulkanPipeline.h"

namespace Engine
{
    std::unique_ptr<PipelineManager> PipelineManager::m_pInstance;

    std::unique_ptr<PipelineManager>& PipelineManager::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new PipelineManager());
        }
        return m_pInstance;
    }

    void PipelineManager::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        m_mPipelines.emplace(EPipelineType::eGraphicsDiffuse, PipelineFactory::CreatePipeline(device, swapchain, EPipelineType::eGraphicsDiffuse));
    }

    void PipelineManager::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->RecreatePipeline(device, swapchain);
        }
    }

    void PipelineManager::Cleanup(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Cleanup(device);
        }
    }

    void PipelineManager::Destroy(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Destroy(device);
        }
    }

    std::unique_ptr<PipelineBase>& PipelineManager::Get(EPipelineType eType)
    {
        auto it = m_mPipelines.find(eType);
        if(it != m_mPipelines.end())
            return it->second;
        return std::unique_ptr<PipelineBase>(nullptr);
    }
}