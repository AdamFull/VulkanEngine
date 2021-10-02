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
                                 std::shared_ptr<Texture2D> normal,
                                 std::shared_ptr<Texture2D> specular);
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

    protected:
        virtual void CreateDescriptorSetLayout() {}
        virtual void CreateDescriptorPool() {}
        virtual void CreateDescriptorSets();
        virtual void CreatePipelineLayout() {}
        vk::DescriptorSetLayout m_descriptorSetLayout;
        vk::DescriptorPool m_descriptorPool;
        std::vector<vk::DescriptorSet> m_vDescriptorSets;

        vk::PipelineLayout m_pipelineLayout;
        std::shared_ptr<PipelineBase> m_pPipeline;
        std::shared_ptr<Texture2D> m_pColor;
    };

    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal,
                                 std::shared_ptr<Texture2D> specular) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        void CreateDescriptorSetLayout() override;
        void CreateDescriptorPool() override;
        void CreateDescriptorSets() override;
        void CreatePipelineLayout() override;
    private:
        std::shared_ptr<Texture2D> m_pAmbient;
        std::shared_ptr<Texture2D> m_pSpecular;
        std::shared_ptr<Texture2D> m_pNormal;
    };
    
}