#include "FilesystemHelper.h"
#include <filesystem>
#include "external/ktx/lib/vk_format.h"

namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include "external/gltf/stb_image.h"

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

    bool FilesystemHelper::LoadImage(char const *filename, ktxTexture** target, vk::Format* format)
    {
        fs::path filepath{filename};

        if(filepath.extension() == ".ktx")
            return LoadKTX(filename, target, format);
        else
            return LoadSTB(filename, target, format);

        return false;
    }

    void FilesystemHelper::CloseImage(ktxTexture** target)
    {
        ktxTexture_Destroy((*target));
    }

    bool FilesystemHelper::AllocateRawDataAsKTXTexture(unsigned char* data, ktxTexture** target, vk::Format* format, int width, int height, bool calcMips)
    {
        bool result = AllocateRawDataAsKTXTexture(target, format, width, height, calcMips);

        (*target)->dataSize = width * height * 4;
        (*target)->pData = static_cast<unsigned char*>(calloc((*target)->dataSize, sizeof(unsigned char)));
        memcpy((*target)->pData, data, (*target)->dataSize);

        return result;
    }

    bool FilesystemHelper::AllocateRawDataAsKTXTexture(ktxTexture** target, vk::Format* format, int width, int height, bool calcMips=false)
    {
        ktxTextureCreateInfo info;
        info.glInternalformat = GL_SRGB8_ALPHA8;
        info.baseWidth = static_cast<uint32_t>(width);
        info.baseHeight = static_cast<uint32_t>(height);
        info.baseDepth = 1;
        info.numLevels = calcMips ? static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1 : 1;
        info.numDimensions = 2;
        info.generateMipmaps = calcMips;
        info.isArray = false;
        info.numLayers = 1;
        info.numFaces = 1;

        auto result = ktxTexture_Create(&info, KTX_TEXTURE_CREATE_NO_STORAGE, target);
        (*target)->baseDepth = 1;

        VkFormat raw_format = vkGetFormatFromOpenGLFormat((*target)->glFormat, (*target)->glType);
        *format = static_cast<vk::Format>(raw_format);

        return result == KTX_SUCCESS;
    }

    bool FilesystemHelper::LoadSTB(char const *filename, ktxTexture** target, vk::Format* format)
    {
        int w, h, c;
        unsigned char* data = stbi_load(filename, &w, &h, &c, STBI_rgb_alpha);

        bool result = AllocateRawDataAsKTXTexture(data, target, format, w, h, true);
        *format = vk::Format::eR8G8B8A8Srgb;

        return result;
    }

    bool FilesystemHelper::LoadKTX(char const *filename, ktxTexture** target, vk::Format* format)
    {
        auto result = ktxTexture_CreateFromNamedFile(filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, target);

        VkFormat raw_format = vkGetFormatFromOpenGLFormat((*target)->glFormat, (*target)->glType);
        *format = static_cast<vk::Format>(raw_format);

        return result == KTX_SUCCESS;
    }
}