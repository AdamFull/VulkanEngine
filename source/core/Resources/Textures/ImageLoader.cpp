#include "ImageLoader.h"
#include <filesystem>
namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include "external/tinygltf/stb_image.h"

using namespace Engine::Resources::Loaders;

bool ImageLoader::Load(char const *filename, ktxTexture **target, vk::Format *format)
{
    fs::path filepath{filename};

    if (filepath.extension() == ".ktx")
        return LoadKTX(filename, target, format);
    else
        return LoadSTB(filename, target, format);

    return false;
}

void ImageLoader::Close(ktxTexture **target)
{
    ktxTexture_Destroy((*target));
}

bool ImageLoader::AllocateRawDataAsKTXTexture(unsigned char *data, ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat, bool calcMips)
{
    bool result = AllocateRawDataAsKTXTexture(target, format, width, height, depth, dims, overrideFormat, calcMips);

    (*target)->dataSize = width * height * 4;
    (*target)->pData = static_cast<unsigned char *>(calloc((*target)->dataSize, sizeof(unsigned char)));
    memcpy((*target)->pData, data, (*target)->dataSize);

    return result;
}

bool ImageLoader::AllocateRawDataAsKTXTexture(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat, bool calcMips)
{
    ktxTextureCreateInfo info;
    info.glInternalformat = overrideFormat;
    info.baseWidth = width;
    info.baseHeight = height;
    info.baseDepth = depth;
    info.numLevels = calcMips ? static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1 : 1;
    info.numDimensions = dims;
    ;
    info.generateMipmaps = calcMips;
    info.isArray = false;
    info.numLayers = 1;
    info.numFaces = 1;

    auto result = ktxTexture_Create(&info, KTX_TEXTURE_CREATE_NO_STORAGE, target);
    (*target)->baseDepth = depth;
    (*target)->dataSize = width * height * (depth > 1 ? depth : 4);

    VkFormat raw_format = vkGetFormatFromOpenGLInternalFormat((*target)->glInternalformat);
    *format = static_cast<vk::Format>(raw_format);

    return result == KTX_SUCCESS;
}

bool ImageLoader::AllocateRawDataAsKTXTextureCubemap(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat, bool calcMips)
{
    ktxTextureCreateInfo info;
    info.glInternalformat = overrideFormat;
    info.baseWidth = width;
    info.baseHeight = height;
    info.baseDepth = depth;
    info.numLevels = calcMips ? static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1 : 1;
    info.numDimensions = dims;
    ;
    info.generateMipmaps = calcMips;
    info.isArray = false;
    info.numLayers = 1;
    info.numFaces = 6;

    auto result = ktxTexture_Create(&info, KTX_TEXTURE_CREATE_NO_STORAGE, target);
    (*target)->baseDepth = depth;
    (*target)->dataSize = width * height * (depth > 1 ? depth : 4);

    VkFormat raw_format = vkGetFormatFromOpenGLInternalFormat((*target)->glInternalformat);
    *format = static_cast<vk::Format>(raw_format);

    return result == KTX_SUCCESS;
}

bool ImageLoader::LoadSTB(char const *filename, ktxTexture **target, vk::Format *format)
{
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, STBI_rgb_alpha);

    bool result = AllocateRawDataAsKTXTexture(data, target, format, w, h, 1, 2, GL_SRGB8_ALPHA8, true);
    *format = vk::Format::eR8G8B8A8Srgb;

    return result;
}

bool ImageLoader::LoadKTX(char const *filename, ktxTexture **target, vk::Format *format)
{
    auto result = ktxTexture_CreateFromNamedFile(filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, target);

    VkFormat raw_format = vkGetFormatFromOpenGLInternalFormat((*target)->glInternalformat);
    *format = static_cast<vk::Format>(raw_format);

    return result == KTX_SUCCESS;
}