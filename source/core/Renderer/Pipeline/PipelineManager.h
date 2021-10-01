#pragma once
#include "PipelineFactory.h"

#define UPipelineMGR PipelineManager::GetInstance()
#define UGPipelineDiffuse UPipelineMGR->Get(EPipelineType::eGraphicsDiffuse)

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
    
    private:
        std::map<EPipelineType, std::unique_ptr<PipelineBase>> m_mPipelines; 
    };
}