#include "PixelFormat.h"

using namespace engine::core;

EPixelFormat FPixel::getPixelFormat(EPixelBitFormat eBitFormat, EBitPerPixel eBitPerPixel, EPixelComponents ePixComp)
{
    switch(eBitFormat)
    {
        case EPixelBitFormat::eUnorm: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e8: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR8Unorm; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR8G8Unorm; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR8G8B8Unorm; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR8G8B8A8Unorm; break;
                    }
                } break;
                case EBitPerPixel::e16: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR16Unorm; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR16G16Unorm; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR16G16B16Unorm; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR16G16B16A16Unorm; break;
                    }
                } break;
                default: return EPixelFormat::eUndefined;
            }
        } break;
        case EPixelBitFormat::eSnorm: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e8: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR8Snorm; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR8G8Snorm; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR8G8B8Snorm; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR8G8B8A8Snorm; break;
                    }
                } break;
                case EBitPerPixel::e16: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR16Snorm; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR16G16Snorm; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR16G16B16Snorm; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR16G16B16A16Snorm; break;
                    }
                } break;
                default: return EPixelFormat::eUndefined;
            }
        } break;
        case EPixelBitFormat::eUint: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e8: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR8Uint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR8G8Uint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR8G8B8Uint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR8G8B8A8Uint; break;
                    }
                } break;
                case EBitPerPixel::e16: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR16Uint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR16G16Uint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR16G16B16Uint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR16G16B16A16Uint; break;
                    }
                } break;
                case EBitPerPixel::e32: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR32Uint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR32G32Uint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR32G32B32Uint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR32G32B32A32Uint; break;
                    }
                } break;
                case EBitPerPixel::e64: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR64Uint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR64G64Uint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR64G64B64Uint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR64G64B64A64Uint; break;
                    }
                } break;
            }
        } break;
        case EPixelBitFormat::eSint: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e8: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR8Sint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR8G8Sint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR8G8B8Sint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR8G8B8A8Sint; break;
                    }
                } break;
                case EBitPerPixel::e16: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR16Sint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR16G16Sint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR16G16B16Sint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR16G16B16A16Sint; break;
                    }
                } break;
                case EBitPerPixel::e32: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR32Sint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR32G32Sint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR32G32B32Sint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR32G32B32A32Sint; break;
                    }
                } break;
                case EBitPerPixel::e64: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR64Sint; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR64G64Sint; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR64G64B64Sint; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR64G64B64A64Sint; break;
                    }
                } break;
            }
        } break;
        case EPixelBitFormat::eSrgb: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e8: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR8Srgb; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR8G8Srgb; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR8G8B8Srgb; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR8G8B8A8Srgb; break;
                    }
                } break;
                default: return EPixelFormat::eUndefined;
            }
        } break;
        case EPixelBitFormat::eSfloat: {
            switch(eBitPerPixel)
            {
                case EBitPerPixel::e16: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR16Sfloat; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR16G16Sfloat; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR16G16B16Sfloat; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR16G16B16A16Sfloat; break;
                    }
                } break;
                case EBitPerPixel::e32: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR32Sfloat; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR32G32Sfloat; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR32G32B32Sfloat; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR32G32B32A32Sfloat; break;
                    }
                } break;
                case EBitPerPixel::e64: {
                    switch(ePixComp)
                    {
                        case EPixelComponents::eR: return EPixelFormat::eR64Sfloat; break;
                        case EPixelComponents::eRG: return EPixelFormat::eR64G64Sfloat; break;
                        case EPixelComponents::eRGB: return EPixelFormat::eR64G64B64Sfloat; break;
                        case EPixelComponents::eRGBA: return EPixelFormat::eR64G64B64A64Sfloat; break;
                    }
                } break;
                default: return EPixelFormat::eUndefined;
            }
        } break;
    }

    return EPixelFormat::eUndefined;
}

#ifdef VULKAN_API_SUPPORT
utl::bimap<EPixelFormat, vk::Format> FPixel::vkFormatConverter{
    {EPixelFormat::eUndefined, vk::Format::eUndefined},
    // 8 bits per component
    {EPixelFormat::eR8Unorm, vk::Format::eR8Unorm},
    {EPixelFormat::eR8G8Unorm, vk::Format::eR8G8Unorm},
    {EPixelFormat::eR8G8B8Unorm, vk::Format::eR8G8B8Unorm},
    {EPixelFormat::eR8G8B8A8Unorm, vk::Format::eR8G8B8A8Unorm},

    {EPixelFormat::eR8Snorm, vk::Format::eR8Snorm},
    {EPixelFormat::eR8G8Snorm, vk::Format::eR8G8Snorm},
    {EPixelFormat::eR8G8B8Snorm, vk::Format::eR8G8B8Snorm},
    {EPixelFormat::eR8G8B8A8Snorm, vk::Format::eR8G8B8A8Snorm},

    {EPixelFormat::eR8Uint, vk::Format::eR8Uint},
    {EPixelFormat::eR8G8Uint, vk::Format::eR8G8Uint},
    {EPixelFormat::eR8G8B8Uint, vk::Format::eR8G8B8Uint},
    {EPixelFormat::eR8G8B8A8Uint, vk::Format::eR8G8B8A8Uint},

    {EPixelFormat::eR8Sint, vk::Format::eR8Sint},
    {EPixelFormat::eR8G8Sint, vk::Format::eR8G8Sint},
    {EPixelFormat::eR8G8B8Sint, vk::Format::eR8G8B8Sint},
    {EPixelFormat::eR8G8B8A8Sint, vk::Format::eR8G8B8A8Sint},

    {EPixelFormat::eR8Srgb, vk::Format::eR8Srgb},
    {EPixelFormat::eR8G8Srgb, vk::Format::eR8G8Srgb},
    {EPixelFormat::eR8G8B8Srgb, vk::Format::eR8G8B8Srgb},
    {EPixelFormat::eR8G8B8A8Srgb, vk::Format::eR8G8B8A8Srgb},

    // 16 bits per component
    {EPixelFormat::eR16Unorm, vk::Format::eR16Unorm},
    {EPixelFormat::eR16G16Unorm, vk::Format::eR16G16Unorm},
    {EPixelFormat::eR16G16B16Unorm, vk::Format::eR16G16B16Unorm},
    {EPixelFormat::eR16G16B16A16Unorm, vk::Format::eR16G16B16A16Unorm},

    {EPixelFormat::eR16Snorm, vk::Format::eR16Snorm},
    {EPixelFormat::eR16G16Snorm, vk::Format::eR16G16Snorm},
    {EPixelFormat::eR16G16B16Snorm, vk::Format::eR16G16B16Snorm},
    {EPixelFormat::eR16G16B16A16Snorm, vk::Format::eR16G16B16A16Snorm},

    {EPixelFormat::eR16Uint, vk::Format::eR16Uint},
    {EPixelFormat::eR16G16Uint, vk::Format::eR16G16Uint},
    {EPixelFormat::eR16G16B16Uint, vk::Format::eR16G16B16Uint},
    {EPixelFormat::eR16G16B16A16Uint, vk::Format::eR16G16B16A16Uint},

    {EPixelFormat::eR16Sint, vk::Format::eR16Sint},
    {EPixelFormat::eR16G16Sint, vk::Format::eR16G16Sint},
    {EPixelFormat::eR16G16B16Sint, vk::Format::eR16G16B16Sint},
    {EPixelFormat::eR16G16B16A16Sint, vk::Format::eR16G16B16A16Sint},

    {EPixelFormat::eR16Sfloat, vk::Format::eR16Sfloat},
    {EPixelFormat::eR16G16Sfloat, vk::Format::eR16G16Sfloat},
    {EPixelFormat::eR16G16B16Sfloat, vk::Format::eR16G16B16Sfloat},
    {EPixelFormat::eR16G16B16A16Sfloat, vk::Format::eR16G16B16A16Sfloat},

    // 32 bits per component
    {EPixelFormat::eR32Uint, vk::Format::eR32Uint},
    {EPixelFormat::eR32G32Uint, vk::Format::eR32G32Uint},
    {EPixelFormat::eR32G32B32Uint, vk::Format::eR32G32B32Uint},
    {EPixelFormat::eR32G32B32A32Uint, vk::Format::eR32G32B32A32Uint},

    {EPixelFormat::eR32Sint, vk::Format::eR32Sint},
    {EPixelFormat::eR32G32Sint, vk::Format::eR32G32Sint},
    {EPixelFormat::eR32G32B32Sint, vk::Format::eR32G32B32Sint},
    {EPixelFormat::eR32G32B32A32Sint, vk::Format::eR32G32B32A32Sint},

    {EPixelFormat::eR32Sfloat, vk::Format::eR32Sfloat},
    {EPixelFormat::eR32G32Sfloat, vk::Format::eR32G32Sfloat},
    {EPixelFormat::eR32G32B32Sfloat, vk::Format::eR32G32B32Sfloat},
    {EPixelFormat::eR32G32B32A32Sfloat, vk::Format::eR32G32B32A32Sfloat},

    // Packed
    {EPixelFormat::eR5G5B5A1UnormPack16, vk::Format::eR5G5B5A1UnormPack16},
    {EPixelFormat::eR5G6B5UnormPack16, vk::Format::eR5G6B5UnormPack16},
    {EPixelFormat::eA2R10G10B10UnormPack32, vk::Format::eA2R10G10B10UnormPack32},
    {EPixelFormat::eR4G4B4A4UnormPack16, vk::Format::eR4G4B4A4UnormPack16},
    {EPixelFormat::eA1R5G5B5UnormPack16, vk::Format::eA1R5G5B5UnormPack16},
    {EPixelFormat::eA2R10G10B10UintPack32, vk::Format::eA2R10G10B10UintPack32},
    {EPixelFormat::eB10G11R11UfloatPack32, vk::Format::eB10G11R11UfloatPack32},
    {EPixelFormat::eE5B9G9R9UfloatPack32, vk::Format::eE5B9G9R9UfloatPack32},

    // S3TC/DXT/BC
    {EPixelFormat::eBc1RgbUnormBlock, vk::Format::eBc1RgbUnormBlock},
    {EPixelFormat::eBc1RgbaUnormBlock, vk::Format::eBc1RgbaUnormBlock},
    {EPixelFormat::eBc2UnormBlock, vk::Format::eBc2UnormBlock},
    {EPixelFormat::eBc3UnormBlock, vk::Format::eBc3UnormBlock},

    {EPixelFormat::eBc1RgbSrgbBlock, vk::Format::eBc1RgbSrgbBlock},
    {EPixelFormat::eBc1RgbaSrgbBlock, vk::Format::eBc1RgbaSrgbBlock},
    {EPixelFormat::eBc2SrgbBlock, vk::Format::eBc2SrgbBlock},
    {EPixelFormat::eBc3SrgbBlock, vk::Format::eBc3SrgbBlock},

    {EPixelFormat::eBc4UnormBlock, vk::Format::eBc4UnormBlock},
    {EPixelFormat::eBc5UnormBlock, vk::Format::eBc5UnormBlock},
    {EPixelFormat::eBc4SnormBlock, vk::Format::eBc4SnormBlock},
    {EPixelFormat::eBc5SnormBlock, vk::Format::eBc5SnormBlock},

    {EPixelFormat::eBc6HUfloatBlock, vk::Format::eBc6HUfloatBlock},
    {EPixelFormat::eBc6HSfloatBlock, vk::Format::eBc6HSfloatBlock},
    {EPixelFormat::eBc7UnormBlock, vk::Format::eBc7UnormBlock},
    {EPixelFormat::eBc7SrgbBlock, vk::Format::eBc7SrgbBlock},

    // ETC
    {EPixelFormat::eEtc2R8G8B8UnormBlock, vk::Format::eEtc2R8G8B8UnormBlock},
    {EPixelFormat::eEtc2R8G8B8A1UnormBlock, vk::Format::eEtc2R8G8B8A1UnormBlock},
    {EPixelFormat::eEtc2R8G8B8A8UnormBlock, vk::Format::eEtc2R8G8B8A8UnormBlock},

    {EPixelFormat::eEtc2R8G8B8SrgbBlock, vk::Format::eEtc2R8G8B8SrgbBlock},
    {EPixelFormat::eEtc2R8G8B8A1SrgbBlock, vk::Format::eEtc2R8G8B8A1SrgbBlock},
    {EPixelFormat::eEtc2R8G8B8A8SrgbBlock, vk::Format::eEtc2R8G8B8A8SrgbBlock},

    {EPixelFormat::eEacR11UnormBlock, vk::Format::eEacR11UnormBlock},
    {EPixelFormat::eEacR11G11UnormBlock, vk::Format::eEacR11G11UnormBlock},
    {EPixelFormat::eEacR11SnormBlock, vk::Format::eEacR11SnormBlock},
    {EPixelFormat::eEacR11G11SnormBlock, vk::Format::eEacR11G11SnormBlock},

    // PVRTC
    {EPixelFormat::ePvrtc12BppUnormBlockIMG, vk::Format::ePvrtc12BppUnormBlockIMG},
    {EPixelFormat::ePvrtc14BppUnormBlockIMG, vk::Format::ePvrtc14BppUnormBlockIMG},
    {EPixelFormat::ePvrtc22BppUnormBlockIMG, vk::Format::ePvrtc22BppUnormBlockIMG},
    {EPixelFormat::ePvrtc24BppUnormBlockIMG, vk::Format::ePvrtc24BppUnormBlockIMG},

    {EPixelFormat::ePvrtc12BppSrgbBlockIMG, vk::Format::ePvrtc12BppSrgbBlockIMG},
    {EPixelFormat::ePvrtc14BppSrgbBlockIMG, vk::Format::ePvrtc14BppSrgbBlockIMG},
    {EPixelFormat::ePvrtc22BppSrgbBlockIMG, vk::Format::ePvrtc22BppSrgbBlockIMG},
    {EPixelFormat::ePvrtc24BppSrgbBlockIMG, vk::Format::ePvrtc24BppSrgbBlockIMG},

    // ASTC
    {EPixelFormat::eAstc4x4UnormBlock, vk::Format::eAstc4x4UnormBlock},
    {EPixelFormat::eAstc5x4UnormBlock, vk::Format::eAstc5x4UnormBlock},
    {EPixelFormat::eAstc5x5UnormBlock, vk::Format::eAstc5x5UnormBlock},
    {EPixelFormat::eAstc6x5UnormBlock, vk::Format::eAstc6x5UnormBlock},
    {EPixelFormat::eAstc6x6UnormBlock, vk::Format::eAstc6x6UnormBlock},
    {EPixelFormat::eAstc8x5UnormBlock, vk::Format::eAstc8x5UnormBlock},
    {EPixelFormat::eAstc8x6UnormBlock, vk::Format::eAstc8x6UnormBlock},
    {EPixelFormat::eAstc8x8UnormBlock, vk::Format::eAstc8x8UnormBlock},
    {EPixelFormat::eAstc10x5UnormBlock, vk::Format::eAstc10x5UnormBlock},
    {EPixelFormat::eAstc10x6UnormBlock, vk::Format::eAstc10x6UnormBlock},
    {EPixelFormat::eAstc10x8UnormBlock, vk::Format::eAstc10x8UnormBlock},
    {EPixelFormat::eAstc10x10UnormBlock, vk::Format::eAstc10x10UnormBlock},
    {EPixelFormat::eAstc12x10UnormBlock, vk::Format::eAstc12x10UnormBlock},
    {EPixelFormat::eAstc12x12UnormBlock, vk::Format::eAstc12x12UnormBlock},

    {EPixelFormat::eAstc4x4SrgbBlock, vk::Format::eAstc4x4SrgbBlock},
    {EPixelFormat::eAstc5x4SrgbBlock, vk::Format::eAstc5x4SrgbBlock},
    {EPixelFormat::eAstc5x5SrgbBlock, vk::Format::eAstc5x5SrgbBlock},
    {EPixelFormat::eAstc6x5SrgbBlock, vk::Format::eAstc6x5SrgbBlock},
    {EPixelFormat::eAstc6x6SrgbBlock, vk::Format::eAstc6x6SrgbBlock},
    {EPixelFormat::eAstc8x5SrgbBlock, vk::Format::eAstc8x5SrgbBlock},
    {EPixelFormat::eAstc8x6SrgbBlock, vk::Format::eAstc8x6SrgbBlock},
    {EPixelFormat::eAstc8x8SrgbBlock, vk::Format::eAstc8x8SrgbBlock},
    {EPixelFormat::eAstc10x5SrgbBlock, vk::Format::eAstc10x5SrgbBlock},
    {EPixelFormat::eAstc10x6SrgbBlock, vk::Format::eAstc10x6SrgbBlock},
    {EPixelFormat::eAstc10x8SrgbBlock, vk::Format::eAstc10x8SrgbBlock},
    {EPixelFormat::eAstc10x10SrgbBlock, vk::Format::eAstc10x10SrgbBlock},
    {EPixelFormat::eAstc12x10SrgbBlock, vk::Format::eAstc12x10SrgbBlock},
    {EPixelFormat::eAstc12x12SrgbBlock, vk::Format::eAstc12x12SrgbBlock},

    // Depth/stencil
    {EPixelFormat::eD16Unorm, vk::Format::eD16Unorm},
    {EPixelFormat::eX8D24UnormPack32, vk::Format::eX8D24UnormPack32},
    {EPixelFormat::eD32Sfloat, vk::Format::eD32Sfloat},
    {EPixelFormat::eS8Uint, vk::Format::eS8Uint},
    {EPixelFormat::eD24UnormS8Uint, vk::Format::eD24UnormS8Uint},
    {EPixelFormat::eD32SfloatS8Uint, vk::Format::eD32SfloatS8Uint}
};

vk::Format FPixel::getVkFormat(const EPixelFormat& format)
{
    if(vkFormatConverter.has_key(format))
        return vkFormatConverter.get_value(format);
    return vk::Format::eUndefined;
}

EPixelFormat FPixel::getUniversalFormat(const vk::Format& format)
{
    if(vkFormatConverter.has_value(format))
        return vkFormatConverter.get_key(format);
    return EPixelFormat::eUndefined;
}
#endif

#ifdef OPENGL_API_SUPPORT
#include "KTX-Software/lib/gl_format.h"
utl::bimap<EPixelFormat, uint32_t> FPixel::glFormatConverter{
    {EPixelFormat::eUndefined, 0},
    //
    // 8 bits per component
    //
    {EPixelFormat::eR8Unorm, GL_R8},
    {EPixelFormat::eR8G8Unorm, GL_RG8},
    {EPixelFormat::eR8G8B8Unorm, GL_RGB8},
    {EPixelFormat::eR8G8B8A8Unorm, GL_RGBA8},

    {EPixelFormat::eR8Snorm, GL_R8_SNORM},
    {EPixelFormat::eR8G8Snorm, GL_RG8_SNORM},
    {EPixelFormat::eR8G8B8Snorm, GL_RGB8_SNORM},
    {EPixelFormat::eR8G8B8A8Snorm, GL_RGBA8_SNORM},

    {EPixelFormat::eR8Uint, GL_R8UI},
    {EPixelFormat::eR8G8Uint, GL_RG8UI},
    {EPixelFormat::eR8G8B8Uint, GL_RGB8UI},
    {EPixelFormat::eR8G8B8A8Uint, GL_RGBA8UI},

    {EPixelFormat::eR8Sint, GL_R8I},
    {EPixelFormat::eR8G8Sint, GL_RG8I},
    {EPixelFormat::eR8G8B8Sint, GL_RGB8I},
    {EPixelFormat::eR8G8B8A8Sint, GL_RGBA8I},

    {EPixelFormat::eR8Srgb, GL_SR8},
    {EPixelFormat::eR8G8Srgb, GL_SRG8},
    {EPixelFormat::eR8G8B8Srgb, GL_SRGB8},
    {EPixelFormat::eR8G8B8A8Srgb, GL_SRGB8_ALPHA8},

    //
    // 16 bits per component
    //
    {EPixelFormat::eR16Unorm, GL_R16},
    {EPixelFormat::eR16G16Unorm, GL_RG16},
    {EPixelFormat::eR16G16B16Unorm, GL_RGB16},
    {EPixelFormat::eR16G16B16A16Unorm, GL_RGBA16},

    {EPixelFormat::eR16Snorm, GL_R16_SNORM},
    {EPixelFormat::eR16G16Snorm, GL_RG16_SNORM},
    {EPixelFormat::eR16G16B16Snorm, GL_RGB16_SNORM},
    {EPixelFormat::eR16G16B16A16Snorm, GL_RGBA16_SNORM},

    {EPixelFormat::eR16Uint, GL_R16UI},
    {EPixelFormat::eR16G16Uint, GL_RG16UI},
    {EPixelFormat::eR16G16B16Uint, GL_RGB16UI},
    {EPixelFormat::eR16G16B16A16Uint, GL_RGBA16UI},

    {EPixelFormat::eR16Sint, GL_R16I},
    {EPixelFormat::eR16G16Sint, GL_RG16I},
    {EPixelFormat::eR16G16B16Sint, GL_RGB16I},
    {EPixelFormat::eR16G16B16A16Sint, GL_RGBA16I},

    {EPixelFormat::eR16Sfloat, GL_R16F},
    {EPixelFormat::eR16G16Sfloat, GL_RG16F},
    {EPixelFormat::eR16G16B16Sfloat, GL_RGB16F},
    {EPixelFormat::eR16G16B16A16Sfloat, GL_RGBA16F},

    //
    // 32 bits per component
    //
    {EPixelFormat::eR32Uint, GL_R32UI},
    {EPixelFormat::eR32G32Uint, GL_RG32UI},
    {EPixelFormat::eR32G32B32Uint, GL_RGB32UI},
    {EPixelFormat::eR32G32B32A32Uint, GL_RGBA32UI},

    {EPixelFormat::eR32Sint, GL_R32I},
    {EPixelFormat::eR32G32Sint, GL_RG32I},
    {EPixelFormat::eR32G32B32Sint, GL_RGB32I},
    {EPixelFormat::eR32G32B32A32Sint, GL_RGBA32I},

    {EPixelFormat::eR32Sfloat, GL_R32F},
    {EPixelFormat::eR32G32Sfloat, GL_RG32F},
    {EPixelFormat::eR32G32B32Sfloat, GL_RGB32F},
    {EPixelFormat::eR32G32B32A32Sfloat, GL_RGBA32F},

    //
    // Packed
    //
    {EPixelFormat::eR5G5B5A1UnormPack16, GL_RGB5},
    {EPixelFormat::eR5G6B5UnormPack16, GL_RGB565},
    {EPixelFormat::eA2R10G10B10UnormPack32, GL_RGB10},
    {EPixelFormat::eR4G4B4A4UnormPack16, GL_RGBA4},
    {EPixelFormat::eA1R5G5B5UnormPack16, GL_RGB5_A1},
    {EPixelFormat::eA2R10G10B10UintPack32, GL_RGB10_A2UI},
    {EPixelFormat::eB10G11R11UfloatPack32, GL_R11F_G11F_B10F},
    {EPixelFormat::eE5B9G9R9UfloatPack32, GL_RGB9_E5},

    //
    // S3TC/DXT/BC
    //
    {EPixelFormat::eBc1RgbUnormBlock, GL_COMPRESSED_RGB_S3TC_DXT1_EXT},
    {EPixelFormat::eBc1RgbaUnormBlock, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
    {EPixelFormat::eBc2UnormBlock, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
    {EPixelFormat::eBc3UnormBlock, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},

    {EPixelFormat::eBc1RgbSrgbBlock, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT},
    {EPixelFormat::eBc1RgbaSrgbBlock, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT},
    {EPixelFormat::eBc2SrgbBlock, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT},
    {EPixelFormat::eBc3SrgbBlock, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT},

    {EPixelFormat::eBc4UnormBlock, GL_COMPRESSED_LUMINANCE_LATC1_EXT},
    {EPixelFormat::eBc5UnormBlock, GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT},
    {EPixelFormat::eBc4SnormBlock, GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT},
    {EPixelFormat::eBc5SnormBlock, GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT},

    {EPixelFormat::eBc6HUfloatBlock, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT},
    {EPixelFormat::eBc6HSfloatBlock, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT},
    {EPixelFormat::eBc7UnormBlock, GL_COMPRESSED_RGBA_BPTC_UNORM},
    {EPixelFormat::eBc7SrgbBlock, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM},

    //
    // ETC
    //
    {EPixelFormat::eEtc2R8G8B8UnormBlock, GL_COMPRESSED_RGB8_ETC2},
    {EPixelFormat::eEtc2R8G8B8A1UnormBlock, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2},
    {EPixelFormat::eEtc2R8G8B8A8UnormBlock, GL_COMPRESSED_RGBA8_ETC2_EAC},

    {EPixelFormat::eEtc2R8G8B8SrgbBlock, GL_COMPRESSED_SRGB8_ETC2},
    {EPixelFormat::eEtc2R8G8B8A1SrgbBlock, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2},
    {EPixelFormat::eEtc2R8G8B8A8SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC},

    {EPixelFormat::eEacR11UnormBlock, GL_COMPRESSED_R11_EAC},
    {EPixelFormat::eEacR11G11UnormBlock, GL_COMPRESSED_RG11_EAC},
    {EPixelFormat::eEacR11SnormBlock, GL_COMPRESSED_SIGNED_R11_EAC},
    {EPixelFormat::eEacR11G11SnormBlock, GL_COMPRESSED_SIGNED_RG11_EAC},

    //
    // PVRTC
    //
    {EPixelFormat::ePvrtc12BppUnormBlockIMG, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG},
    {EPixelFormat::ePvrtc14BppUnormBlockIMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG},
    {EPixelFormat::ePvrtc22BppUnormBlockIMG, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG},
    {EPixelFormat::ePvrtc24BppUnormBlockIMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG},

    {EPixelFormat::ePvrtc12BppSrgbBlockIMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT},
    {EPixelFormat::ePvrtc14BppSrgbBlockIMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT},
    {EPixelFormat::ePvrtc22BppSrgbBlockIMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG},
    {EPixelFormat::ePvrtc24BppSrgbBlockIMG, GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG},

    //
    // ASTC
    //
    {EPixelFormat::eAstc4x4UnormBlock, GL_COMPRESSED_RGBA_ASTC_4x4_KHR},
    {EPixelFormat::eAstc5x4UnormBlock, GL_COMPRESSED_RGBA_ASTC_5x4_KHR},
    {EPixelFormat::eAstc5x5UnormBlock, GL_COMPRESSED_RGBA_ASTC_5x5_KHR},
    {EPixelFormat::eAstc6x5UnormBlock, GL_COMPRESSED_RGBA_ASTC_6x5_KHR},
    {EPixelFormat::eAstc6x6UnormBlock, GL_COMPRESSED_RGBA_ASTC_6x6_KHR},
    {EPixelFormat::eAstc8x5UnormBlock, GL_COMPRESSED_RGBA_ASTC_8x5_KHR},
    {EPixelFormat::eAstc8x6UnormBlock, GL_COMPRESSED_RGBA_ASTC_8x6_KHR},
    {EPixelFormat::eAstc8x8UnormBlock, GL_COMPRESSED_RGBA_ASTC_8x8_KHR},
    {EPixelFormat::eAstc10x5UnormBlock, GL_COMPRESSED_RGBA_ASTC_10x5_KHR},
    {EPixelFormat::eAstc10x6UnormBlock, GL_COMPRESSED_RGBA_ASTC_10x6_KHR},
    {EPixelFormat::eAstc10x8UnormBlock, GL_COMPRESSED_RGBA_ASTC_10x8_KHR},
    {EPixelFormat::eAstc10x10UnormBlock, GL_COMPRESSED_RGBA_ASTC_10x10_KHR},
    {EPixelFormat::eAstc12x10UnormBlock, GL_COMPRESSED_RGBA_ASTC_12x10_KHR},
    {EPixelFormat::eAstc12x12UnormBlock, GL_COMPRESSED_RGBA_ASTC_12x12_KHR},

    {EPixelFormat::eAstc4x4SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR},
    {EPixelFormat::eAstc5x4SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR},
    {EPixelFormat::eAstc5x5SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR},
    {EPixelFormat::eAstc6x5SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR},
    {EPixelFormat::eAstc6x6SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR},
    {EPixelFormat::eAstc8x5SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR},
    {EPixelFormat::eAstc8x6SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR},
    {EPixelFormat::eAstc8x8SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR},
    {EPixelFormat::eAstc10x5SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR},
    {EPixelFormat::eAstc10x6SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR},
    {EPixelFormat::eAstc10x8SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR},
    {EPixelFormat::eAstc10x10SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR},
    {EPixelFormat::eAstc12x10SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR},
    {EPixelFormat::eAstc12x12SrgbBlock, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR},

    //
    // Depth/stencil
    //
    {EPixelFormat::eD16Unorm, GL_DEPTH_COMPONENT16},
    {EPixelFormat::eX8D24UnormPack32, GL_DEPTH_COMPONENT24},
    {EPixelFormat::eD32Sfloat, GL_DEPTH_COMPONENT32F},
    {EPixelFormat::eS8Uint, GL_STENCIL_INDEX8},
    {EPixelFormat::eD24UnormS8Uint, GL_DEPTH24_STENCIL8},
    {EPixelFormat::eD32SfloatS8Uint, GL_DEPTH32F_STENCIL8}
};

uint32_t FPixel::getGlFormat(const EPixelFormat& format)
{
    if(glFormatConverter.has_key(format))
        return glFormatConverter.get_value(format);
    return 0;
}

EPixelFormat FPixel::getUniversalFormat(const uint32_t& format)
{
    if(glFormatConverter.has_value(format))
        return glFormatConverter.get_key(format);
    return EPixelFormat::eUndefined;
}
#endif

#if defined(VULKAN_API_SUPPORT) && defined(OPENGL_API_SUPPORT)
        vk::Format FPixel::getVkFormatFromGlFormat(uint32_t format)
        {
            auto pixFormat = glFormatConverter.get_key(format);
            return vkFormatConverter.get_value(pixFormat);
        }

        uint32_t FPixel::getGlFormatFromVkFormat(vk::Format format)
        {
            auto pixFormat = vkFormatConverter.get_key(format);
            return glFormatConverter.get_value(pixFormat);
        }
#endif
