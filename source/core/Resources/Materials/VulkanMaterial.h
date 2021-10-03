#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class Texture2D;
    class PipelineBase;

    class MaterialBase : public ResourceBase
    {
    public:
        virtual void Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal = nullptr,
                                 std::shared_ptr<Texture2D> specular = nullptr);
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

    protected:
        virtual void CreateDescriptorSetLayout() {}
        virtual void CreateDescriptorPool(uint32_t images) {}
        virtual void CreateDescriptorSets(uint32_t images);
        virtual void CreatePipelineCache();
        virtual void CreatePipelineLayout(uint32_t images);
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        std::vector<vk::DescriptorSet> vDescriptorSets;

        vk::PipelineLayout pipelineLayout;
        vk::PipelineCache pipelineCache;
        std::shared_ptr<PipelineBase> m_pPipeline;
        
        std::shared_ptr<Texture2D> m_pColor;
        std::shared_ptr<Texture2D> m_pAmbient;
        std::shared_ptr<Texture2D> m_pSpecular;
        std::shared_ptr<Texture2D> m_pNormal;
    };    
}