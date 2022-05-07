#pragma once
#include "Image.h"
#include "noise/FractalNoise.hpp"

namespace Engine
{
    namespace Core
    {
        /**
         * @brief Implementation of 3d image
         * 
         */
        class CImage3D : public CImage
        {
        public:
            CImage3D() = default;

            void loadNoise(Resources::ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth);

        protected:
            void generatePerlinNoise(ktxTexture *texture);
            void generateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation);
        };
    }
}