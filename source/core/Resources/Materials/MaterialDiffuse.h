#pragma once
#include "VulkanMaterial.h"

namespace Engine
{
    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create(std::shared_ptr<Texture2D> color, 
                                 std::shared_ptr<Texture2D> normal = nullptr,
                                 std::shared_ptr<Texture2D> specular = nullptr) override;
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;
    protected:
        void CreateDescriptorSetLayout() override;
        void CreateDescriptorPool(uint32_t images) override;
        void CreateDescriptorSets(uint32_t images) override;
        void CreatePipelineCache() override;
        void CreatePipelineLayout(uint32_t images) override;
    };
}