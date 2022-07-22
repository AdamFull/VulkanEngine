#pragma once

namespace engine
{
    namespace core
    {
        enum class EPixelFormat
        {
            eUndefined,

            // 8 bits per component
            eR8Unorm,
            eR8G8Unorm,
            eR8G8B8Unorm,
            eR8G8B8A8Unorm,

            eR8Snorm,
            eR8G8Snorm,
            eR8G8B8Snorm,
            eR8G8B8A8Snorm,

            eR8Uint,
            eR8G8Uint,
            eR8G8B8Uint,
            eR8G8B8A8Uint,

            eR8Sint,
            eR8G8Sint,
            eR8G8B8Sint,
            eR8G8B8A8Sint,

            eR8Srgb,
            eR8G8Srgb,
            eR8G8B8Srgb,
            eR8G8B8A8Srgb,

            // 16 bits per component
            eR16Unorm,
            eR16G16Unorm,
            eR16G16B16Unorm,
            eR16G16B16A16Unorm,

            eR16Snorm,
            eR16G16Snorm,
            eR16G16B16Snorm,
            eR16G16B16A16Snorm,

            eR16Uint,
            eR16G16Uint,
            eR16G16B16Uint,
            eR16G16B16A16Uint,

            eR16Sint,
            eR16G16Sint,
            eR16G16B16Sint,
            eR16G16B16A16Sint,

            eR16Sfloat,
            eR16G16Sfloat,
            eR16G16B16Sfloat,
            eR16G16B16A16Sfloat,

            // 32 bits per component
            eR32Uint,
            eR32G32Uint,
            eR32G32B32Uint,
            eR32G32B32A32Uint,

            eR32Sint,
            eR32G32Sint,
            eR32G32B32Sint,
            eR32G32B32A32Sint,

            eR32Sfloat,
            eR32G32Sfloat,
            eR32G32B32Sfloat,
            eR32G32B32A32Sfloat,

            // 64 bits per component
            eR64Uint,
            eR64G64Uint,
            eR64G64B64Uint,
            eR64G64B64A64Uint,

            eR64Sint,
            eR64G64Sint,
            eR64G64B64Sint,
            eR64G64B64A64Sint,

            eR64Sfloat,
            eR64G64Sfloat,
            eR64G64B64Sfloat,
            eR64G64B64A64Sfloat,

            // Packed
            eR5G5B5A1UnormPack16,
            eR5G6B5UnormPack16,
            eA2R10G10B10UnormPack32,
            eR4G4B4A4UnormPack16,
            eA1R5G5B5UnormPack16,
            eA2R10G10B10UintPack32,
            eB10G11R11UfloatPack32,
            eE5B9G9R9UfloatPack32,

            // S3TC/DXT/BC
            eBc1RgbUnormBlock,
            eBc1RgbaUnormBlock,
            eBc2UnormBlock,
            eBc3UnormBlock,

            eBc1RgbSrgbBlock,
            eBc1RgbaSrgbBlock,
            eBc2SrgbBlock,
            eBc3SrgbBlock,

            eBc4UnormBlock,
            eBc5UnormBlock,
            eBc4SnormBlock,
            eBc5SnormBlock,

            eBc6HUfloatBlock,
            eBc6HSfloatBlock,
            eBc7UnormBlock,
            eBc7SrgbBlock,

            // ETC
            eEtc2R8G8B8UnormBlock,
            eEtc2R8G8B8A1UnormBlock,
            eEtc2R8G8B8A8UnormBlock,

            eEtc2R8G8B8SrgbBlock,
            eEtc2R8G8B8A1SrgbBlock,
            eEtc2R8G8B8A8SrgbBlock,

            eEacR11UnormBlock,
            eEacR11G11UnormBlock,
            eEacR11SnormBlock,
            eEacR11G11SnormBlock,

            // PVRTC
            ePvrtc12BppUnormBlockIMG,
            ePvrtc14BppUnormBlockIMG,
            ePvrtc22BppUnormBlockIMG,
            ePvrtc24BppUnormBlockIMG,

            ePvrtc12BppSrgbBlockIMG,
            ePvrtc14BppSrgbBlockIMG,
            ePvrtc22BppSrgbBlockIMG,
            ePvrtc24BppSrgbBlockIMG,

            // ASTC
            eAstc4x4UnormBlock,
            eAstc5x4UnormBlock,
            eAstc5x5UnormBlock,
            eAstc6x5UnormBlock,
            eAstc6x6UnormBlock,
            eAstc8x5UnormBlock,
            eAstc8x6UnormBlock,
            eAstc8x8UnormBlock,
            eAstc10x5UnormBlock,
            eAstc10x6UnormBlock,
            eAstc10x8UnormBlock,
            eAstc10x10UnormBlock,
            eAstc12x10UnormBlock,
            eAstc12x12UnormBlock,

            eAstc4x4SrgbBlock,
            eAstc5x4SrgbBlock,
            eAstc5x5SrgbBlock,
            eAstc6x5SrgbBlock,
            eAstc6x6SrgbBlock,
            eAstc8x5SrgbBlock,
            eAstc8x6SrgbBlock,
            eAstc8x8SrgbBlock,
            eAstc10x5SrgbBlock,
            eAstc10x6SrgbBlock,
            eAstc10x8SrgbBlock,
            eAstc10x10SrgbBlock,
            eAstc12x10SrgbBlock,
            eAstc12x12SrgbBlock,

            // Depth/stencil
            eD16Unorm,
            eX8D24UnormPack32,
            eD32Sfloat,
            eS8Uint,
            eD24UnormS8Uint,
            eD32SfloatS8Uint
        };

        enum class EBitPerPixel
        {
            e8, e16, e32, e64
        };

        enum class EPixelBitFormat
        {
            eUnorm, eSnorm, eUint, eSint, eSrgb, eSfloat
        };

        enum class EPixelComponents
        {
            eR = 1, eRG, eRGB, eRGBA
        };

        struct FPixel
        {
            static EPixelFormat getPixelFormat(EPixelBitFormat eBitFormat, EBitPerPixel eBitPerPixel, EPixelComponents ePixComp);

#ifdef VULKAN_API_SUPPORT
            static utl::bimap<EPixelFormat, vk::Format> vkFormatConverter;
            static vk::Format getVkFormat(const EPixelFormat& format);
            static EPixelFormat getUniversalFormat(const vk::Format& format);
#endif

#ifdef OPENGL_API_SUPPORT
            static utl::bimap<EPixelFormat, uint32_t> glFormatConverter;
            static uint32_t getGlFormat(const EPixelFormat& format);
            static EPixelFormat getUniversalFormat(const uint32_t& format);
#endif

#if defined(VULKAN_API_SUPPORT) && defined(OPENGL_API_SUPPORT)
            static vk::Format getVkFormatFromGlFormat(uint32_t format);
            static uint32_t getGlFormatFromVkFormat(vk::Format format);
#endif
            
        };
    }
}