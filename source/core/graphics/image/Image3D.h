#pragma once
#include "Image.h"
#include "noise/FractalNoise.hpp"

namespace Engine
{
    namespace Core
    {
        class Image3D : public Image
        {
        public:
            Image3D() = default;

            void LoadNoise(Resources::ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth);

        protected:
            void GeneratePerlinNoise(ktxTexture *texture);
            void GenerateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation);
        };
    }
}