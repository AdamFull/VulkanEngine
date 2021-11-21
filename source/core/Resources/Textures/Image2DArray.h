#pragma once
#include "Image.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class Image2DArray : public Image
            {
            public:
                Image2DArray() = default;

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;
            };
        }
    }
}