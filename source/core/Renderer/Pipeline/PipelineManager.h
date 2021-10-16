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

        std::unique_ptr<PipelineBase>& Get(EPipelineType eType);
        inline vk::DescriptorSetLayout& GetDescriptorSetLayout() { return descriptorSetLayout; }
        inline vk::DescriptorPool& GetDescriptorPool() { return descriptorPool; }
        inline vk::PipelineLayout& GetPipelineLayout() { return pipelineLayout; }
        inline vk::PipelineCache& GetPipelineCache() { return pipelineCache; }
    private:
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool(uint32_t images);
        void CreatePipelineLayout(uint32_t images);
        void CreatePipelineCache();

        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        vk::PipelineLayout pipelineLayout;
        vk::PipelineCache pipelineCache;
        std::map<EPipelineType, std::unique_ptr<PipelineBase>> m_mPipelines; 
    };
}