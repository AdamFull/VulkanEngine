#pragma once
#include "PipelineFactory.h"

#define UPipelineMGR PipelineManager::GetInstance()
#define UGPipelineDiffuse UPipelineMGR->Get(EPipelineType::eGraphicsDiffuse)
#define UPMGR_DSL UPipelineMGR->GetDescriptorSetLayout()
#define UPMGR_DP UPipelineMGR->GetDescriptorPool()
#define UPMGR_PL UPipelineMGR->GetPipelineLayout()
#define UPMGR_PC UPipelineMGR->GetPipelineCache()

namespace Engine
{
    class Device;
    class SwapChain;

    class PipelineManager
    {
        PipelineManager() = default;
        static std::unique_ptr<PipelineManager> m_pInstance;
    public:
        PipelineManager(const PipelineManager&) = delete;
        void operator=(const PipelineManager&) = delete;
        PipelineManager(PipelineManager&&) = delete;
        PipelineManager& operator=(PipelineManager&&) = delete;

        static std::unique_ptr<PipelineManager>& GetInstance();
    public:
        void Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);
        void Cleanup(std::unique_ptr<Device>& device);
        void Destroy(std::unique_ptr<Device>& device);

        std::shared_ptr<PipelineBase>& Get(EShaderSet eType);
        inline vk::DescriptorSetLayout& GetDescriptorSetLayout() { return descriptorSetLayout; }
        inline vk::DescriptorPool& GetDescriptorPool() { return descriptorPool; }
        inline vk::PipelineLayout& GetPipelineLayout() { return pipelineLayout; }
        inline vk::PipelineCache& GetPipelineCache() { return pipelineCache; }
    private:
        FPipelineCreateInfo CreateInfo(std::unique_ptr<Device>& device, EShaderSet eSet);
        std::map<EShaderSet, std::shared_ptr<PipelineBase>> m_mPipelines; 
    };
}