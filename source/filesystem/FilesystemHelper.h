#pragma once
#include "external/ktx/include/ktx.h"

namespace Engine
{
    struct FilesystemHelper
    {
        static std::vector<char> ReadFile(const std::string& srPAth);
        static bool IsFileExist(const std::string& srPath);

        static bool LoadImage(char const *filename, ktxTexture** target, vk::Format* format);
        static void CloseImage(ktxTexture** target);
        static bool AllocateRawDataAsKTXTexture(unsigned char* data, ktxTexture** target, vk::Format* format, int width, int height, bool calcMips=false);
    private:
        static bool LoadSTB(char const *filename, ktxTexture** target, vk::Format* format);
        static bool LoadKTX(char const *filename, ktxTexture** target, vk::Format* format);
    };
}