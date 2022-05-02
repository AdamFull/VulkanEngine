#pragma once
#include "external/ktx/include/ktx.h"
#include "external/ktx/lib/vk_format.h"

namespace Engine
{
    namespace Core
    {
        namespace Loaders
        {
            struct CImageLoader
            {
            public:
                static bool load(char const *filename, ktxTexture **target, vk::Format *format);
                static void close(ktxTexture **target);
                static bool allocateRawDataAsKTXTexture(unsigned char *data, ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);
                static bool allocateRawDataAsKTXTextureCubemap(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);
                static bool allocateRawDataAsKTXTexture(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);

            private:
                static bool loadSTB(char const *filename, ktxTexture **target, vk::Format *format);
                static bool loadKTX(char const *filename, ktxTexture **target, vk::Format *format);
            };
        }
    }

}