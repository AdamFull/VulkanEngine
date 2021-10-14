#include "FilesystemHelper.h"
#include <filesystem>

namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include "external/gltf/stb_image.h"

#include "external/ktx/include/ktx.h"

namespace Engine
{
    std::vector<char> FilesystemHelper::ReadFile(const std::string& srPath)
    {
        std::ifstream file(srPath, std::ios_base::in | std::ios_base::binary);
        return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    bool FilesystemHelper::IsFileExist(const std::string& srPath)
    {
        return fs::exists(srPath);
    }

    bool FilesystemHelper::LoadImage(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo)
    {
        fs::path filepath{filename};

        if(filepath.extension() == ".ktx")
            return LoadKTX(raw_data, filename, imageInfo);
        else
            return LoadSTB(raw_data, filename, imageInfo);

        return false;
    }

    bool FilesystemHelper::LoadSTB(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo)
    {
        int w, h, c;
        unsigned char* data = stbi_load(filename, &w, &h, &c, STBI_rgb_alpha);

        imageInfo->format = vk::Format::eR8G8B8A8Srgb;
        imageInfo->width = static_cast<uint32_t>(w);
        imageInfo->height = static_cast<uint32_t>(h);
        imageInfo->channels = static_cast<uint32_t>(c);
        imageInfo->size = static_cast<uint32_t>(w * h * 4);
        imageInfo->mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(w, h)))) + 1;
        imageInfo->layers = 1;
        imageInfo->faces = 1;

        (*raw_data) = static_cast<unsigned char*>(calloc(imageInfo->size, sizeof(unsigned char)));
        memcpy((*raw_data), data, imageInfo->size * sizeof(unsigned char));
        stbi_image_free(data);

        return (*raw_data) != nullptr;
    }

    bool FilesystemHelper::LoadKTX(unsigned char** raw_data, char const *filename, FImageLoadInfo* imageInfo)
    {
        ktxTexture** target;
        //auto result = ktxTexture_CreateFromNamedFile(filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, target);
        return true;
    }
}