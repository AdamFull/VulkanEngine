#include "ImageLoaderNew.h"

#include "KTX-Software/include/ktx.h"
#include "KTX-Software/include/ktxvulkan.h"

using namespace engine::core;

void CImageLoaderNew::load(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI, const std::vector<EPixelFormat>& supportedFormats, const fs::path &parentPath)
{
    auto fsFullPath = parentPath / fsPath;
    auto texFormat = getTextureFormat(fsPath);
    switch(texFormat)
    {
        case EImageFormat::eKTX:  { loadKTX(fsFullPath, imageCI); } break;
        case EImageFormat::eKTX2: { loadKTX2(fsFullPath, imageCI, supportedFormats); } break;
        case EImageFormat::eDDS: { loadDDS(fsFullPath, imageCI); } break;

        default: break;
    }
}

EImageFormat CImageLoaderNew::getTextureFormat(const fs::path& fsPath)
{
    static std::map<std::string, EImageFormat> imageFormatMapStatic
    {
        {".ktx", EImageFormat::eKTX},
        {".ktx2", EImageFormat::eKTX2},
        {".dds", EImageFormat::eDDS}
    };

    auto ext = fsPath.extension().string();
    auto it = imageFormatMapStatic.find(ext);
    if(it != imageFormatMapStatic.end())
        return it->second;

    return EImageFormat::eUnknown;
}

void CImageLoaderNew::loadKTX(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI)
{
    ktxTexture* kTexture{nullptr};
    imageCI = utl::make_scope<FImageCreateInfo>();

    auto ktxresult = ktxTexture_CreateFromNamedFile(fsPath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);
    assert(ktxresult == KTX_SUCCESS && "Failed to open ktx texture.");

    imageCI->isArray = kTexture->isArray;
    imageCI->isCubemap = kTexture->isCubemap;
    imageCI->isCompressed = kTexture->isCompressed;
    imageCI->generateMipmaps = kTexture->generateMipmaps;
    imageCI->baseWidth = kTexture->baseWidth;
    imageCI->baseHeight = kTexture->baseHeight;
    imageCI->baseDepth = kTexture->baseDepth;
    imageCI->numDimensions = kTexture->numDimensions;
    imageCI->numLevels = kTexture->numLevels;
    imageCI->numLayers = kTexture->numLayers;
    imageCI->numFaces = kTexture->numFaces;

    imageCI->dataSize = kTexture->dataSize;
    imageCI->pData = utl::make_scope<uint8_t[]>(imageCI->dataSize);
    std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);

    auto vkFormat = static_cast<vk::Format>(ktxTexture_GetVkFormat(kTexture));
    imageCI->pixFormat = FPixel::getUniversalFormat(vkFormat);

    auto numLayers = imageCI->isCubemap ? imageCI->numFaces : imageCI->numLayers;
    for (uint32_t layer = 0; layer < numLayers; layer++)
    {
        for (uint32_t level = 0; level < imageCI->numLevels; level++)
        {
            size_t offset{0};
            ktxresult = ktxTexture_GetImageOffset(kTexture, level, 0, layer, &offset);
            assert(ktxresult == KTX_SUCCESS && "Failed to load mip level.");
            imageCI->mipOffsets[layer].emplace_back(offset);
        }
    }

    ktxTexture_Destroy(kTexture);
}

void CImageLoaderNew::loadKTX2(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI, const std::vector<EPixelFormat>& supportedFormats)
{
    ktxTexture2* kTexture{nullptr};
    imageCI = utl::make_scope<FImageCreateInfo>();

    auto ktxresult = ktxTexture_CreateFromNamedFile(fsPath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**)&kTexture);
    assert(ktxresult == KTX_SUCCESS && "Failed to open ktx texture.");

    imageCI->isArray = kTexture->isArray;
    imageCI->isCubemap = kTexture->isCubemap;
    imageCI->isCompressed = kTexture->isCompressed;
    imageCI->generateMipmaps = kTexture->generateMipmaps;
    imageCI->baseWidth = kTexture->baseWidth;
    imageCI->baseHeight = kTexture->baseHeight;
    imageCI->baseDepth = kTexture->baseDepth;
    imageCI->numDimensions = kTexture->numDimensions;
    imageCI->numLevels = kTexture->numLevels;
    imageCI->numLayers = kTexture->numLayers;
    imageCI->numFaces = kTexture->numFaces;

    if (ktxTexture2_NeedsTranscoding(kTexture))
    {
        ktxresult = ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_ASTC_4x4_RGBA, 0);
        assert(ktxresult == KTX_SUCCESS && "Failed to transcode texture.");
    }

    imageCI->dataSize = kTexture->dataSize;
    imageCI->pData = utl::make_scope<uint8_t[]>(imageCI->dataSize);
    std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);

    auto vkFormat = static_cast<vk::Format>(kTexture->vkFormat);
    imageCI->pixFormat = FPixel::getUniversalFormat(vkFormat);

    auto numLayers = imageCI->isCubemap ? imageCI->numFaces : imageCI->numLayers;
    for (uint32_t layer = 0; layer < numLayers; layer++)
    {
        for (uint32_t level = 0; level < imageCI->numLevels; level++)
        {
            size_t offset{0};
            ktxresult = ktxTexture_GetImageOffset((ktxTexture*)kTexture, level, 0, layer, &offset);
            assert(ktxresult == KTX_SUCCESS && "Failed to load mip level.");
            imageCI->mipOffsets[layer].emplace_back(offset);
        }
    }

    ktxTexture_Destroy((ktxTexture*)kTexture);
}

void CImageLoaderNew::loadDDS(const fs::path& fsPath, utl::scope_ptr<FImageCreateInfo>& imageCI)
{

}
