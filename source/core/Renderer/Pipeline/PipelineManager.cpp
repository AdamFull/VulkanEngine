#include "PipelineManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "VulkanPipeline.h"

namespace Engine
{
    constexpr std::array<EShaderSet, 4> aShaderSets { EShaderSet::eUI, EShaderSet::eDiffuse, EShaderSet::eSkybox, EShaderSet::ePBR };
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
        uint32_t images = swapchain->GetImages().size();

        for(auto& eSet : aShaderSets)
        {
            m_mPipelines.emplace(eSet, PipelineFactory::CreatePipeline(CreateInfo(device, eSet), device, swapchain));
        }
    }

    void PipelineManager::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        uint32_t images = swapchain->GetImages().size();
        m_pInstance->CreateDescriptorSetLayout(device);
        m_pInstance->CreateDescriptorPool(device, images);
        m_pInstance->CreatePipelineLayout(device, images);
        m_pInstance->CreatePipelineCache(device);

        for(auto& [key, value] : m_mPipelines)
        {
            value->RecreatePipeline(CreateInfo(device, key), device, swapchain);
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

    std::shared_ptr<PipelineBase>& PipelineManager::Get(EShaderSet eType)
    {
        auto it = m_mPipelines.find(eType);
        if(it != m_mPipelines.end())
            return it->second;
        return std::shared_ptr<PipelineBase>(nullptr);
    }

    FPipelineCreateInfo PipelineManager::CreateInfo(std::unique_ptr<Device>& device, EShaderSet eSet)
    {
        switch (eSet)
        {
            case EShaderSet::eUI: return PipelineConfig::CreateUIPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eDiffuse: return PipelineConfig::CreateDiffusePipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eSkybox: return PipelineConfig::CreateSkyboxPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::ePBR: return PipelineConfig::CreatePBRPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
        }

        return FPipelineCreateInfo{};
    }
}