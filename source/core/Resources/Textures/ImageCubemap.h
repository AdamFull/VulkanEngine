#pragma once
#include "Image.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class ImageCubemap : public Image
            {
            public:
                ImageCubemap() = default;

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
            };
        }
    }
}