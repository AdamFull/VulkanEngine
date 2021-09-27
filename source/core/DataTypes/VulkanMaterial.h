#pragma once

namespace Engine
{
    class VulkanTextureBase;
    class Device;
    class Pipeline;

    class MaterialBase
    {
    public:
        virtual void Create(std::unique_ptr<Device>& device) {}

        virtual void Bind(vk::CommandBuffer& commandBuffer) {}
        virtual void Draw(vk::CommandBuffer& commandBuffer) {}
    protected:
        std::unique_ptr<Pipeline> m_pPipeline;
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