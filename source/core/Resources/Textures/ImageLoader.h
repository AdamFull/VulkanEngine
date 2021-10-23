#pragma once
#include "external/ktx/include/ktx.h"
#include "external/ktx/lib/vk_format.h"

namespace Engine
{
    struct ImageLoader
    {
    public:
        static bool Load(char const *filename, ktxTexture** target, vk::Format* format);
        static void Close(ktxTexture** target);
        static bool AllocateRawDataAsKTXTexture(unsigned char* data, ktxTexture** target, vk::Format* format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips=false);
        static bool AllocateRawDataAsKTXTexture(ktxTexture** target, vk::Format* format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips=false);
    private:
        static bool LoadSTB(char const *filename, ktxTexture** target, vk::Format* format);
        static bool LoadKTX(char const *filename, ktxTexture** target, vk::Format* format);
    };
}