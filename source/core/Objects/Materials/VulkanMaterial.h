#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    class VulkanTextureBase;
    class Device;
    class GraphicsPipelineDiffuse;

    class MaterialBase : public RenderObject
    {
    public:
        virtual void Create(std::unique_ptr<Device>& device) {}

        virtual void Bind(vk::CommandBuffer& commandBuffer) {}
        virtual void Draw(vk::CommandBuffer& commandBuffer) {}
    protected:
        std::shared_ptr<GraphicsPipelineDiffuse> m_pPipeline;
        std::shared_ptr<VulkanTextureBase> m_pColor;
    };

    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create(std::unique_ptr<Device>& device) override;

        void Bind(vk::CommandBuffer& commandBuffer) override;
        void Draw(vk::CommandBuffer& commandBuffer) override;
    private:
        std::shared_ptr<VulkanTextureBase> m_pAmbient;
        std::shared_ptr<VulkanTextureBase> m_pSpecular;
        std::shared_ptr<VulkanTextureBase> m_pNormal;
    };
    
}