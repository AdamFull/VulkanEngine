#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class TextureBase;
    class PipelineBase;

    class MaterialBase : public ResourceBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

    protected:
        std::shared_ptr<PipelineBase> m_pPipeline;
        std::shared_ptr<TextureBase> m_pColor;
    };

    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

    private:
        std::shared_ptr<TextureBase> m_pAmbient;
        std::shared_ptr<TextureBase> m_pSpecular;
        std::shared_ptr<TextureBase> m_pNormal;
    };
    
}