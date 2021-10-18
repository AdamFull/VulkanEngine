#pragma once
#include "VulkanTexture.h"
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    class Texture3D : public TextureBase
    {
    public:
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        void LoadNoise(ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth);
    protected:
        void GeneratePerlinNoise(ktxTexture* texture);
        void GenerateFractalNoise(ktxTexture* texture, uint32_t octaves, float perceptation);
        uint32_t GetInternalFormat() override { return 0x8229; } //GL_R8, for texture generators
    };
}