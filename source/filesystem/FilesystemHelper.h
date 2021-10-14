#pragma once

namespace Engine
{
    struct FImageLoadInfo
    {
        vk::Format format;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        uint32_t size;
        uint32_t mipLevels;
        uint32_t layers;
        uint32_t faces;
    };

    struct FilesystemHelper
    {
        static std::vector<char> ReadFile(const std::string& srPAth);
        static bool IsFileExist(const std::string& srPath);

        static bool LoadImage(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo);
    private:
        static bool LoadSTB(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo);
        static bool LoadKTX(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo);
    };
}