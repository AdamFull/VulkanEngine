#pragma once
#include "Image.h"
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class Image3D : public Image
            {
            public:
                Image3D() = default;

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

                void LoadNoise(ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth);

            protected:
                void GeneratePerlinNoise(ktxTexture *texture);
                void GenerateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation);
            };
        }
    }
}