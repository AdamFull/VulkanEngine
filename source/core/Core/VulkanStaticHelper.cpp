#include "VulkanStaticHelper.h"
#include "external/ktx/lib/vk_format.h"

using namespace Engine::Core;

#ifdef NDEBUG
const bool VulkanStaticHelper::m_bEnableValidationLayers = false;
#else
const bool VulkanStaticHelper::m_bEnableValidationLayers = true;
#endif

const std::vector<const char *> VulkanStaticHelper::m_vValidationLayers =
    {
        "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> VulkanStaticHelper::m_vDeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool VulkanStaticHelper::CheckValidationLayerSupport()
{
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char *layerName : m_vValidationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            std::cout << layerProperties.layerName << std::endl;
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanStaticHelper::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_bEnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanStaticHelper::CheckDeviceExtensionSupport(const vk::PhysicalDevice &device)
{
    std::set<std::string> sRequiredExtensions(m_vDeviceExtensions.begin(), m_vDeviceExtensions.end());

    for (const auto &extension : device.enumerateDeviceExtensionProperties())
    {
        sRequiredExtensions.erase(extension.extensionName);
    }

    return sRequiredExtensions.empty();
}

vk::SurfaceFormatKHR VulkanStaticHelper::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
    {
        return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
    }

    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR VulkanStaticHelper::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
{
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

uint32_t VulkanStaticHelper::VkFormatToGLFormat(vk::Format format)
{
     switch ( format )
	{
		//
		// 8 bits per component
		//
		case vk::Format::eR8Unorm:										return GL_R8;				// 1-component, 8-bit unsigned normalized
		case vk::Format::eR8G8Unorm:									return GL_RG8;				// 2-component, 8-bit unsigned normalized
		case vk::Format::eR8G8B8Unorm:									return GL_RGB8;			    // 3-component, 8-bit unsigned normalized
		case vk::Format::eR8G8B8A8Unorm:								return GL_RGBA8;			// 4-component, 8-bit unsigned normalized

		case vk::Format::eR8Snorm:										return GL_R8_SNORM;			// 1-component, 8-bit signed normalized
		case vk::Format::eR8G8Snorm:									return GL_RG8_SNORM;		// 2-component, 8-bit signed normalized
		case vk::Format::eR8G8B8Snorm:									return GL_RGB8_SNORM;		// 3-component, 8-bit signed normalized
		case vk::Format::eR8G8B8A8Snorm:								return GL_RGBA8_SNORM;		// 4-component, 8-bit signed normalized

		case vk::Format::eR8Uint:										return GL_R8UI;				// 1-component, 8-bit unsigned integer
		case vk::Format::eR8G8Uint:										return GL_RG8UI;			// 2-component, 8-bit unsigned integer
		case vk::Format::eR8G8B8Uint:									return GL_RGB8UI;			// 3-component, 8-bit unsigned integer
		case vk::Format::eR8G8B8A8Uint:									return GL_RGBA8UI;			// 4-component, 8-bit unsigned integer

		case vk::Format::eR8Sint:										return GL_R8I; 				// 1-component, 8-bit signed integer
		case vk::Format::eR8G8Sint:										return GL_RG8I; 			// 2-component, 8-bit signed integer
		case vk::Format::eR8G8B8Sint:									return GL_RGB8I;			// 3-component, 8-bit signed integer
		case vk::Format::eR8G8B8A8Sint:									return GL_RGBA8I;			// 4-component, 8-bit signed integer

		case vk::Format::eR8Srgb:										return GL_SR8;		  		// 1-component, 8-bit sRGB
		case vk::Format::eR8G8Srgb:										return GL_SRG8;		  		// 2-component, 8-bit sRGB
		case vk::Format::eR8G8B8Srgb:									return GL_SRGB8;			// 3-component, 8-bit sRGB
		case vk::Format::eR8G8B8A8Srgb:									return GL_SRGB8_ALPHA8;		// 4-component, 8-bit sRGB

		//
		// 16 bits per component
		//
		case vk::Format::eR16Unorm:										return GL_R16;              // 1-component, 16-bit unsigned normalized
		case vk::Format::eR16G16Unorm:									return GL_RG16;     	    // 2-component, 16-bit unsigned normalized
		case vk::Format::eR16G16B16Unorm:								return GL_RGB16;    	    // 3-component, 16-bit unsigned normalized
		case vk::Format::eR16G16B16A16Unorm:							return GL_RGBA16;   	    // 4-component, 16-bit unsigned normalized

		case vk::Format::eR16Snorm:										return GL_R16_SNORM;        // 1-component, 16-bit signed normalized
		case vk::Format::eR16G16Snorm:									return GL_RG16_SNORM;  	    // 2-component, 16-bit signed normalized
		case vk::Format::eR16G16B16Snorm:								return GL_RGB16_SNORM; 	    // 3-component, 16-bit signed normalized
		case vk::Format::eR16G16B16A16Snorm:							return GL_RGBA16_SNORM;		// 4-component, 16-bit signed normalized

		case vk::Format::eR16Uint:										return GL_R16UI;		    // 1-component, 16-bit unsigned integer
		case vk::Format::eR16G16Uint:									return GL_RG16UI;		    // 2-component, 16-bit unsigned integer
		case vk::Format::eR16G16B16Uint:								return GL_RGB16UI;  		// 3-component, 16-bit unsigned integer
		case vk::Format::eR16G16B16A16Uint:								return GL_RGBA16UI; 		// 4-component, 16-bit unsigned integer

		case vk::Format::eR16Sint:										return GL_R16I;   	        // 1-component, 16-bit signed integer
		case vk::Format::eR16G16Sint:									return GL_RG16I;  	        // 2-component, 16-bit signed integer
		case vk::Format::eR16G16B16Sint:								return GL_RGB16I; 		    // 3-component, 16-bit signed integer
		case vk::Format::eR16G16B16A16Sint:								return GL_RGBA16I;		    // 4-component, 16-bit signed integer

		case vk::Format::eR16Sfloat:									return GL_R16F;   		    // 1-component, 16-bit floating-point
		case vk::Format::eR16G16Sfloat:									return GL_RG16F;  		    // 2-component, 16-bit floating-point
		case vk::Format::eR16G16B16Sfloat:								return GL_RGB16F; 	        // 3-component, 16-bit floating-point
		case vk::Format::eR16G16B16A16Sfloat:							return GL_RGBA16F;		    // 4-component, 16-bit floating-point

		//
		// 32 bits per component
		//
		case  vk::Format::eR32Uint:										return GL_R32UI;			// 1-component, 32-bit unsigned integer
		case  vk::Format::eR32G32Uint:									return GL_RG32UI;			// 2-component, 32-bit unsigned integer
		case vk::Format::eR32G32B32Uint:								return GL_RGB32UI;  		// 3-component, 32-bit unsigned integer
		case vk::Format::eR32G32B32A32Uint:								return GL_RGBA32UI; 		// 4-component, 32-bit unsigned integer

		case vk::Format::eR32Sint:										return GL_R32I;       		// 1-component, 32-bit signed integer
		case vk::Format::eR32G32Sint:									return GL_RG32I;   			// 2-component, 32-bit signed integer
		case vk::Format::eR32G32B32Sint:								return GL_RGB32I;   		// 3-component, 32-bit signed integer
		case vk::Format::eR32G32B32A32Sint:								return GL_RGBA32I;     		// 4-component, 32-bit signed integer

		case vk::Format::eR32Sfloat:									return GL_R32F;       	    // 1-component, 32-bit floating-point
		case vk::Format::eR32G32Sfloat:									return GL_RG32F;   	        // 2-component, 32-bit floating-point
		case vk::Format::eR32G32B32Sfloat:								return GL_RGB32F;   	    // 3-component, 32-bit floating-point
		case vk::Format::eR32G32B32A32Sfloat:							return GL_RGBA32F;     		// 4-component, 32-bit floating-point

		//
		// Packed
		//
		case vk::Format::eR5G5B5A1UnormPack16:							return GL_RGB5;             	// 3-component 5:5:5,       unsigned normalized
		case vk::Format::eR5G6B5UnormPack16:							return GL_RGB565;               // 3-component 5:6:5,       unsigned normalized
		case vk::Format::eA2R10G10B10UnormPack32:						return GL_RGB10;            	    // 3-component 10:10:10,    unsigned normalized
		case vk::Format::eR4G4B4A4UnormPack16:							return GL_RGBA4;            	// 4-component 4:4:4:4,     unsigned normalized
		case vk::Format::eA1R5G5B5UnormPack16:							return GL_RGB5_A1;          	// 4-component 5:5:5:1,     unsigned normalized
		case vk::Format::eA2R10G10B10UintPack32:						return GL_RGB10_A2UI;           // 4-component 10:10:10:2,  unsigned integer
		case vk::Format::eB10G11R11UfloatPack32:						return GL_R11F_G11F_B10F;       // 3-component 11:11:10,    floating-point
		case vk::Format::eE5B9G9R9UfloatPack32:							return GL_RGB9_E5;              // 3-component/exp 9:9:9/5, floating-point

		//
		// S3TC/DXT/BC
		//

		case vk::Format::eBc1RgbUnormBlock:					            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;     	// line through 3D space, 4x4 blocks, unsigned normalized
		case vk::Format::eBc1RgbaUnormBlock:				            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;    	// line through 3D space plus 1-bit alpha, 4x4 blocks, unsigned normalized
		case vk::Format::eBc2UnormBlock:					            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;    	// line through 3D space plus line through 1D space, 4x4 blocks, unsigned normalized
		case vk::Format::eBc3UnormBlock:					            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;    	// line through 3D space plus 4-bit alpha, 4x4 blocks, unsigned normalized

		case vk::Format::eBc1RgbSrgbBlock:				                return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;        	// line through 3D space, 4x4 blocks, sRGB
		case vk::Format::eBc1RgbaSrgbBlock:				                return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;  	// line through 3D space plus 1-bit alpha, 4x4 blocks, sRGB
		case vk::Format::eBc2SrgbBlock:					                return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;  	// line through 3D space plus line through 1D space, 4x4 blocks, sRGB
		case vk::Format::eBc3SrgbBlock:					                return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;  	// line through 3D space plus 4-bit alpha, 4x4 blocks, sRGB

		case vk::Format::eBc4UnormBlock:	                            return GL_COMPRESSED_LUMINANCE_LATC1_EXT;               			// line through 1D space, 4x4 blocks, unsigned normalized
		case vk::Format::eBc5UnormBlock:	                            return GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT;         			// two lines through 1D space, 4x4 blocks, unsigned normalized
		case vk::Format::eBc4SnormBlock:	                            return GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT;        			// line through 1D space, 4x4 blocks, signed normalized
		case vk::Format::eBc5SnormBlock:	                            return GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT;  			// two lines through 1D space, 4x4 blocks, signed normalized

		case vk::Format::eBc6HUfloatBlock:			                    return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;   		// 3-component, 4x4 blocks, unsigned floating-point
		case vk::Format::eBc6HSfloatBlock:			                    return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;     		// 3-component, 4x4 blocks, signed floating-point
		case vk::Format::eBc7UnormBlock:			                    return GL_COMPRESSED_RGBA_BPTC_UNORM;           	    // 4-component, 4x4 blocks, unsigned normalized
		case vk::Format::eBc7SrgbBlock:			                        return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;     	    // 4-component, 4x4 blocks, sRGB

		//
		// ETC
		//
		case vk::Format::eEtc2R8G8B8UnormBlock:		                    return GL_COMPRESSED_RGB8_ETC2;                             // 3-component ETC2, 4x4 blocks, unsigned normalized
		case vk::Format::eEtc2R8G8B8A1UnormBlock:		                return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2; 	    // 4-component ETC2 with 1-bit alpha, 4x4 blocks, unsigned normalized
		case vk::Format::eEtc2R8G8B8A8UnormBlock:		                return GL_COMPRESSED_RGBA8_ETC2_EAC;                	    // 4-component ETC2, 4x4 blocks, unsigned normalized

		case vk::Format::eEtc2R8G8B8SrgbBlock:	                        return GL_COMPRESSED_SRGB8_ETC2;                            // 3-component ETC2, 4x4 blocks, sRGB
		case vk::Format::eEtc2R8G8B8A1SrgbBlock:	                    return GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;    	// 4-component ETC2 with 1-bit alpha, 4x4 blocks, sRGB
		case vk::Format::eEtc2R8G8B8A8SrgbBlock:	                    return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;             	// 4-component ETC2, 4x4 blocks, sRGB

		case vk::Format::eEacR11UnormBlock:					            return GL_COMPRESSED_R11_EAC;           	                // 1-component ETC, 4x4 blocks, unsigned normalized
		case vk::Format::eEacR11G11UnormBlock:					        return GL_COMPRESSED_RG11_EAC;          	                // 2-component ETC, 4x4 blocks, unsigned normalized
		case vk::Format::eEacR11SnormBlock:						        return GL_COMPRESSED_SIGNED_R11_EAC;    	                // 1-component ETC, 4x4 blocks, signed normalized
		case vk::Format::eEacR11G11SnormBlock:					        return GL_COMPRESSED_SIGNED_RG11_EAC;   	                // 2-component ETC, 4x4 blocks, signed normalized

		//
		// PVRTC
		//
		case vk::Format::ePvrtc12BppUnormBlockIMG:                      return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; 			// 4-component PVRTC, 16x8 blocks, unsigned normalized
		case vk::Format::ePvrtc14BppUnormBlockIMG:                      return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; 			// 4-component PVRTC,  8x8 blocks, unsigned normalized
		case vk::Format::ePvrtc22BppUnormBlockIMG:                      return GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG; 			// 4-component PVRTC,  8x4 blocks, unsigned normalized
		case vk::Format::ePvrtc24BppUnormBlockIMG:                      return GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG; 			// 4-component PVRTC,  4x4 blocks, unsigned normalized

		case vk::Format::ePvrtc12BppSrgbBlockIMG:                       return GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT; 	    // 4-component PVRTC, 16x8 blocks, sRGB
		case vk::Format::ePvrtc14BppSrgbBlockIMG:                       return GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT; 	    // 4-component PVRTC,  8x8 blocks, sRGB
		case vk::Format::ePvrtc22BppSrgbBlockIMG:                       return GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG; 	    // 4-component PVRTC,  8x4 blocks, sRGB
		case vk::Format::ePvrtc24BppSrgbBlockIMG:                       return GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG; 	    // 4-component PVRTC,  4x4 blocks, sRGB

		//
		// ASTC
		//
		case vk::Format::eAstc4x4UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;     	        // 4-component ASTC, 4x4 blocks, unsigned normalized
		case vk::Format::eAstc5x4UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_5x4_KHR;     	        // 4-component ASTC, 5x4 blocks, unsigned normalized
		case vk::Format::eAstc5x5UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_5x5_KHR;     	        // 4-component ASTC, 5x5 blocks, unsigned normalized
		case vk::Format::eAstc6x5UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_6x5_KHR;     	        // 4-component ASTC, 6x5 blocks, unsigned normalized
		case vk::Format::eAstc6x6UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_6x6_KHR;     	        // 4-component ASTC, 6x6 blocks, unsigned normalized
		case vk::Format::eAstc8x5UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_8x5_KHR;     	        // 4-component ASTC, 8x5 blocks, unsigned normalized
		case vk::Format::eAstc8x6UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_8x6_KHR;     	        // 4-component ASTC, 8x6 blocks, unsigned normalized
		case vk::Format::eAstc8x8UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_8x8_KHR;     	        // 4-component ASTC, 8x8 blocks, unsigned normalized
		case vk::Format::eAstc10x5UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_10x5_KHR;    	        // 4-component ASTC, 10x5 blocks, unsigned normalized
		case vk::Format::eAstc10x6UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_10x6_KHR;    	        // 4-component ASTC, 10x6 blocks, unsigned normalized
		case vk::Format::eAstc10x8UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_10x8_KHR;    	        // 4-component ASTC, 10x8 blocks, unsigned normalized
		case vk::Format::eAstc10x10UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_10x10_KHR;   	        // 4-component ASTC, 10x10 blocks, unsigned normalized
		case vk::Format::eAstc12x10UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_12x10_KHR;   	        // 4-component ASTC, 12x10 blocks, unsigned normalized
		case vk::Format::eAstc12x12UnormBlock:					        return GL_COMPRESSED_RGBA_ASTC_12x12_KHR;   	        // 4-component ASTC, 12x12 blocks, unsigned normalized

		case vk::Format::eAstc4x4SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;     	// 4-component ASTC, 4x4 blocks, sRGB
		case vk::Format::eAstc5x4SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;     	// 4-component ASTC, 5x4 blocks, sRGB
		case vk::Format::eAstc5x5SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;     	// 4-component ASTC, 5x5 blocks, sRGB
		case vk::Format::eAstc6x5SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;     	// 4-component ASTC, 6x5 blocks, sRGB
		case vk::Format::eAstc6x6SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;     	// 4-component ASTC, 6x6 blocks, sRGB
		case vk::Format::eAstc8x5SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;     	// 4-component ASTC, 8x5 blocks, sRGB
		case vk::Format::eAstc8x6SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;     	// 4-component ASTC, 8x6 blocks, sRGB
		case vk::Format::eAstc8x8SrgbBlock:			                    return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;     	// 4-component ASTC, 8x8 blocks, sRGB
		case vk::Format::eAstc10x5SrgbBlock:				            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;    	// 4-component ASTC, 10x5 blocks, sRGB
		case vk::Format::eAstc10x6SrgbBlock:				            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;    	// 4-component ASTC, 10x6 blocks, sRGB
		case vk::Format::eAstc10x8SrgbBlock:				            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;    	// 4-component ASTC, 10x8 blocks, sRGB
		case vk::Format::eAstc10x10SrgbBlock:			                return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;   	// 4-component ASTC, 10x10 blocks, sRGB
		case vk::Format::eAstc12x10SrgbBlock:			                return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;   	// 4-component ASTC, 12x10 blocks, sRGB
		case vk::Format::eAstc12x12SrgbBlock:			                return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;   	// 4-component ASTC, 12x12 blocks, sRGB


		//
		// Depth/stencil
		//
		case vk::Format::eD16Unorm:					                    return GL_DEPTH_COMPONENT16;		
		case vk::Format::eX8D24UnormPack32:			                    return GL_DEPTH_COMPONENT24;		
		case vk::Format::eD32Sfloat:				                    return GL_DEPTH_COMPONENT32F;			
		case vk::Format::eS8Uint:					                    return GL_STENCIL_INDEX8;			
		case vk::Format::eD24UnormS8Uint:			                    return GL_DEPTH24_STENCIL8;		    
		case vk::Format::eD32SfloatS8Uint:			                    return GL_DEPTH32F_STENCIL8;		   

		default:												        return 0;
	}
}

vk::Format VulkanStaticHelper::GLFormatToVkFormat(uint32_t format)
{
    switch ( format )
	{
		//
		// 8 bits per component
		//
		case GL_R8:												return vk::Format::eR8Unorm;				// 1-component, 8-bit unsigned normalized
		case GL_RG8:											return vk::Format::eR8G8Unorm;				// 2-component, 8-bit unsigned normalized
		case GL_RGB8:											return vk::Format::eR8G8B8Unorm;			// 3-component, 8-bit unsigned normalized
		case GL_RGBA8:											return vk::Format::eR8G8B8A8Unorm;			// 4-component, 8-bit unsigned normalized

		case GL_R8_SNORM:										return vk::Format::eR8Snorm;				// 1-component, 8-bit signed normalized
		case GL_RG8_SNORM:										return vk::Format::eR8G8Snorm;				// 2-component, 8-bit signed normalized
		case GL_RGB8_SNORM:										return vk::Format::eR8G8B8Snorm;			// 3-component, 8-bit signed normalized
		case GL_RGBA8_SNORM:									return vk::Format::eR8G8B8A8Snorm;			// 4-component, 8-bit signed normalized

		case GL_R8UI:											return vk::Format::eR8Uint;					// 1-component, 8-bit unsigned integer
		case GL_RG8UI:											return vk::Format::eR8G8Uint;				// 2-component, 8-bit unsigned integer
		case GL_RGB8UI:											return vk::Format::eR8G8B8Uint;				// 3-component, 8-bit unsigned integer
		case GL_RGBA8UI:										return vk::Format::eR8G8B8A8Uint;			// 4-component, 8-bit unsigned integer

		case GL_R8I:											return vk::Format::eR8Sint;					// 1-component, 8-bit signed integer
		case GL_RG8I:											return vk::Format::eR8G8Sint;				// 2-component, 8-bit signed integer
		case GL_RGB8I:											return vk::Format::eR8G8B8Sint;				// 3-component, 8-bit signed integer
		case GL_RGBA8I:											return vk::Format::eR8G8B8A8Sint;			// 4-component, 8-bit signed integer

		case GL_SR8:											return vk::Format::eR8Srgb;					// 1-component, 8-bit sRGB
		case GL_SRG8:											return vk::Format::eR8G8Srgb;				// 2-component, 8-bit sRGB
		case GL_SRGB8:											return vk::Format::eR8G8B8Srgb;				// 3-component, 8-bit sRGB
		case GL_SRGB8_ALPHA8:									return vk::Format::eR8G8B8A8Srgb;			// 4-component, 8-bit sRGB

		//
		// 16 bits per component
		//
		case GL_R16:											return vk::Format::eR16Unorm;			    // 1-component, 16-bit unsigned normalized
		case GL_RG16:											return vk::Format::eR16G16Unorm;			// 2-component, 16-bit unsigned normalized
		case GL_RGB16:											return vk::Format::eR16G16B16Unorm;		    // 3-component, 16-bit unsigned normalized
		case GL_RGBA16:											return vk::Format::eR16G16B16A16Unorm;		// 4-component, 16-bit unsigned normalized

		case GL_R16_SNORM:										return vk::Format::eR16Snorm;			    // 1-component, 16-bit signed normalized
		case GL_RG16_SNORM:										return vk::Format::eR16G16Snorm;			// 2-component, 16-bit signed normalized
		case GL_RGB16_SNORM:									return vk::Format::eR16G16B16Snorm;		    // 3-component, 16-bit signed normalized
		case GL_RGBA16_SNORM:									return vk::Format::eR16G16B16A16Snorm;		// 4-component, 16-bit signed normalized

		case GL_R16UI:											return vk::Format::eR16Uint;				// 1-component, 16-bit unsigned integer
		case GL_RG16UI:											return vk::Format::eR16G16Uint;			    // 2-component, 16-bit unsigned integer
		case GL_RGB16UI:										return vk::Format::eR16G16B16Uint;			// 3-component, 16-bit unsigned integer
		case GL_RGBA16UI:										return vk::Format::eR16G16B16A16Uint;		// 4-component, 16-bit unsigned integer

		case GL_R16I:											return vk::Format::eR16Sint;				// 1-component, 16-bit signed integer
		case GL_RG16I:											return vk::Format::eR16G16Sint;			    // 2-component, 16-bit signed integer
		case GL_RGB16I:											return vk::Format::eR16G16B16Sint;			// 3-component, 16-bit signed integer
		case GL_RGBA16I:										return vk::Format::eR16G16B16A16Sint;		// 4-component, 16-bit signed integer

		case GL_R16F:											return vk::Format::eR16Sfloat;				// 1-component, 16-bit floating-point
		case GL_RG16F:											return vk::Format::eR16G16Sfloat;			// 2-component, 16-bit floating-point
		case GL_RGB16F:											return vk::Format::eR16G16B16Sfloat;		// 3-component, 16-bit floating-point
		case GL_RGBA16F:										return vk::Format::eR16G16B16A16Sfloat;		// 4-component, 16-bit floating-point

		//
		// 32 bits per component
		//
		case GL_R32UI:											return vk::Format::eR32Uint;				// 1-component, 32-bit unsigned integer
		case GL_RG32UI:											return vk::Format::eR32G32Uint;				// 2-component, 32-bit unsigned integer
		case GL_RGB32UI:										return vk::Format::eR32G32B32Uint;			// 3-component, 32-bit unsigned integer
		case GL_RGBA32UI:										return vk::Format::eR32G32B32A32Uint;		// 4-component, 32-bit unsigned integer

		case GL_R32I:											return vk::Format::eR32Sint;				// 1-component, 32-bit signed integer
		case GL_RG32I:											return vk::Format::eR32G32Sint;				// 2-component, 32-bit signed integer
		case GL_RGB32I:											return vk::Format::eR32G32B32Sint;			// 3-component, 32-bit signed integer
		case GL_RGBA32I:										return vk::Format::eR32G32B32A32Sint;		// 4-component, 32-bit signed integer

		case GL_R32F:											return vk::Format::eR32Sfloat;				// 1-component, 32-bit floating-point
		case GL_RG32F:											return vk::Format::eR32G32Sfloat;			// 2-component, 32-bit floating-point
		case GL_RGB32F:											return vk::Format::eR32G32B32Sfloat;		// 3-component, 32-bit floating-point
		case GL_RGBA32F:										return vk::Format::eR32G32B32A32Sfloat;		// 4-component, 32-bit floating-point

		//
		// Packed
		//
		case GL_R3_G3_B2:										return vk::Format::eUndefined;					// 3-component 3:3:2,       unsigned normalized
		case GL_RGB4:											return vk::Format::eUndefined;					// 3-component 4:4:4,       unsigned normalized
		case GL_RGB5:											return vk::Format::eR5G5B5A1UnormPack16;		// 3-component 5:5:5,       unsigned normalized
		case GL_RGB565:											return vk::Format::eR5G6B5UnormPack16;		    // 3-component 5:6:5,       unsigned normalized
		case GL_RGB10:											return vk::Format::eA2R10G10B10UnormPack32;	    // 3-component 10:10:10,    unsigned normalized
		case GL_RGB12:											return vk::Format::eUndefined;					// 3-component 12:12:12,    unsigned normalized
		case GL_RGBA2:											return vk::Format::eUndefined;					// 4-component 2:2:2:2,     unsigned normalized
		case GL_RGBA4:											return vk::Format::eR4G4B4A4UnormPack16;		// 4-component 4:4:4:4,     unsigned normalized
		case GL_RGBA12:											return vk::Format::eUndefined;					// 4-component 12:12:12:12, unsigned normalized
		case GL_RGB5_A1:										return vk::Format::eA1R5G5B5UnormPack16;		// 4-component 5:5:5:1,     unsigned normalized
		case GL_RGB10_A2:										return vk::Format::eA2R10G10B10UnormPack32;	    // 4-component 10:10:10:2,  unsigned normalized
		case GL_RGB10_A2UI:										return vk::Format::eA2R10G10B10UintPack32;	    // 4-component 10:10:10:2,  unsigned integer
		case GL_R11F_G11F_B10F:									return vk::Format::eB10G11R11UfloatPack32;	    // 3-component 11:11:10,    floating-point
		case GL_RGB9_E5:										return vk::Format::eE5B9G9R9UfloatPack32;	    // 3-component/exp 9:9:9/5, floating-point

		//
		// S3TC/DXT/BC
		//

		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:					return vk::Format::eBc1RgbUnormBlock;		// line through 3D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:					return vk::Format::eBc1RgbaUnormBlock;		// line through 3D space plus 1-bit alpha, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:					return vk::Format::eBc2UnormBlock;			// line through 3D space plus line through 1D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:					return vk::Format::eBc3UnormBlock;			// line through 3D space plus 4-bit alpha, 4x4 blocks, unsigned normalized

		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:					return vk::Format::eBc1RgbSrgbBlock;		// line through 3D space, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:			return vk::Format::eBc1RgbaSrgbBlock;		// line through 3D space plus 1-bit alpha, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:			return vk::Format::eBc2SrgbBlock;			// line through 3D space plus line through 1D space, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:			return vk::Format::eBc3SrgbBlock;			// line through 3D space plus 4-bit alpha, 4x4 blocks, sRGB

		case GL_COMPRESSED_LUMINANCE_LATC1_EXT:					return vk::Format::eBc4UnormBlock;			// line through 1D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT:			return vk::Format::eBc5UnormBlock;			// two lines through 1D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT:			return vk::Format::eBc4SnormBlock;			// line through 1D space, 4x4 blocks, signed normalized
		case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT:	return vk::Format::eBc5SnormBlock;			// two lines through 1D space, 4x4 blocks, signed normalized

		case GL_COMPRESSED_RED_RGTC1:							return vk::Format::eBc4UnormBlock;			// line through 1D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RG_RGTC2:							return vk::Format::eBc5UnormBlock;			// two lines through 1D space, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_SIGNED_RED_RGTC1:					return vk::Format::eBc4SnormBlock;			// line through 1D space, 4x4 blocks, signed normalized
		case GL_COMPRESSED_SIGNED_RG_RGTC2:						return vk::Format::eBc5SnormBlock;			// two lines through 1D space, 4x4 blocks, signed normalized

		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:				return vk::Format::eBc6HUfloatBlock;		// 3-component, 4x4 blocks, unsigned floating-point
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:				return vk::Format::eBc6HSfloatBlock;		// 3-component, 4x4 blocks, signed floating-point
		case GL_COMPRESSED_RGBA_BPTC_UNORM:						return vk::Format::eBc7UnormBlock;			// 4-component, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:				return vk::Format::eBc7SrgbBlock;			// 4-component, 4x4 blocks, sRGB

		//
		// ETC
		//
		case GL_ETC1_RGB8_OES:									return vk::Format::eEtc2R8G8B8UnormBlock;	// 3-component ETC1, 4x4 blocks, unsigned normalized

		case GL_COMPRESSED_RGB8_ETC2:							return vk::Format::eEtc2R8G8B8UnormBlock;	// 3-component ETC2, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:		return vk::Format::eEtc2R8G8B8A1UnormBlock;	// 4-component ETC2 with 1-bit alpha, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA8_ETC2_EAC:						return vk::Format::eEtc2R8G8B8A8UnormBlock;	// 4-component ETC2, 4x4 blocks, unsigned normalized

		case GL_COMPRESSED_SRGB8_ETC2:							return vk::Format::eEtc2R8G8B8SrgbBlock;	// 3-component ETC2, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:		return vk::Format::eEtc2R8G8B8A1SrgbBlock;	// 4-component ETC2 with 1-bit alpha, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:				return vk::Format::eEtc2R8G8B8A8SrgbBlock;	// 4-component ETC2, 4x4 blocks, sRGB

		case GL_COMPRESSED_R11_EAC:								return vk::Format::eEacR11UnormBlock;		// 1-component ETC, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RG11_EAC:							return vk::Format::eEacR11G11UnormBlock;	// 2-component ETC, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_SIGNED_R11_EAC:						return vk::Format::eEacR11SnormBlock;		// 1-component ETC, 4x4 blocks, signed normalized
		case GL_COMPRESSED_SIGNED_RG11_EAC:						return vk::Format::eEacR11G11SnormBlock;	// 2-component ETC, 4x4 blocks, signed normalized

		//
		// PVRTC
		//
		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG: return vk::Format::ePvrtc12BppUnormBlockIMG;			// 3-component PVRTC, 16x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG: return vk::Format::ePvrtc14BppUnormBlockIMG;			// 3-component PVRTC,  8x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: return vk::Format::ePvrtc12BppUnormBlockIMG;			// 4-component PVRTC, 16x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: return vk::Format::ePvrtc14BppUnormBlockIMG;			// 4-component PVRTC,  8x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG: return vk::Format::ePvrtc22BppUnormBlockIMG;			// 4-component PVRTC,  8x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG: return vk::Format::ePvrtc24BppUnormBlockIMG;			// 4-component PVRTC,  4x4 blocks, unsigned normalized

		case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT: return vk::Format::ePvrtc12BppSrgbBlockIMG;			// 3-component PVRTC, 16x8 blocks, sRGB
		case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT: return vk::Format::ePvrtc14BppSrgbBlockIMG;			// 3-component PVRTC,  8x8 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT: return vk::Format::ePvrtc12BppSrgbBlockIMG;	    // 4-component PVRTC, 16x8 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT: return vk::Format::ePvrtc14BppSrgbBlockIMG;	    // 4-component PVRTC,  8x8 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG: return vk::Format::ePvrtc22BppSrgbBlockIMG;	    // 4-component PVRTC,  8x4 blocks, sRGB
		case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG: return vk::Format::ePvrtc24BppSrgbBlockIMG;	    // 4-component PVRTC,  4x4 blocks, sRGB

		//
		// ASTC
		//
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:					return vk::Format::eAstc4x4UnormBlock;		// 4-component ASTC, 4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:					return vk::Format::eAstc5x4UnormBlock;		// 4-component ASTC, 5x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:					return vk::Format::eAstc5x5UnormBlock;		// 4-component ASTC, 5x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:					return vk::Format::eAstc6x5UnormBlock;		// 4-component ASTC, 6x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:					return vk::Format::eAstc6x6UnormBlock;		// 4-component ASTC, 6x6 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:					return vk::Format::eAstc8x5UnormBlock;		// 4-component ASTC, 8x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:					return vk::Format::eAstc8x6UnormBlock;		// 4-component ASTC, 8x6 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:					return vk::Format::eAstc8x8UnormBlock;		// 4-component ASTC, 8x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:					return vk::Format::eAstc10x5UnormBlock;		// 4-component ASTC, 10x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:					return vk::Format::eAstc10x6UnormBlock;		// 4-component ASTC, 10x6 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:					return vk::Format::eAstc10x8UnormBlock;		// 4-component ASTC, 10x8 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:					return vk::Format::eAstc10x10UnormBlock;	// 4-component ASTC, 10x10 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:					return vk::Format::eAstc12x10UnormBlock;	// 4-component ASTC, 12x10 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:					return vk::Format::eAstc12x12UnormBlock;	// 4-component ASTC, 12x12 blocks, unsigned normalized

		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:			return vk::Format::eAstc4x4SrgbBlock;		// 4-component ASTC, 4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:			return vk::Format::eAstc5x4SrgbBlock;		// 4-component ASTC, 5x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:			return vk::Format::eAstc5x5SrgbBlock;		// 4-component ASTC, 5x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:			return vk::Format::eAstc6x5SrgbBlock;		// 4-component ASTC, 6x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:			return vk::Format::eAstc6x6SrgbBlock;		// 4-component ASTC, 6x6 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:			return vk::Format::eAstc8x5SrgbBlock;		// 4-component ASTC, 8x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:			return vk::Format::eAstc8x6SrgbBlock;		// 4-component ASTC, 8x6 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:			return vk::Format::eAstc8x8SrgbBlock;		// 4-component ASTC, 8x8 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:			return vk::Format::eAstc10x5SrgbBlock;		// 4-component ASTC, 10x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:			return vk::Format::eAstc10x6SrgbBlock;		// 4-component ASTC, 10x6 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:			return vk::Format::eAstc10x8SrgbBlock;		// 4-component ASTC, 10x8 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:			return vk::Format::eAstc10x10SrgbBlock;		// 4-component ASTC, 10x10 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:			return vk::Format::eAstc12x10SrgbBlock;		// 4-component ASTC, 12x10 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:			return vk::Format::eAstc12x12SrgbBlock;		// 4-component ASTC, 12x12 blocks, sRGB

		case GL_COMPRESSED_RGBA_ASTC_3x3x3_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 3x3x3 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_4x3x3_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 4x3x3 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_4x4x3_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 4x4x3 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_4x4x4_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 4x4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_5x4x4_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 5x4x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_5x5x4_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 5x5x4 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_5x5x5_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 5x5x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_6x5x5_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 6x5x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_6x6x5_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 6x6x5 blocks, unsigned normalized
		case GL_COMPRESSED_RGBA_ASTC_6x6x6_OES:					return vk::Format::eUndefined;					// 4-component ASTC, 6x6x6 blocks, unsigned normalized

		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 3x3x3 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 4x3x3 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 4x4x3 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 4x4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 5x4x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 5x5x4 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 5x5x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 6x5x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 6x6x5 blocks, sRGB
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES:			return vk::Format::eUndefined;					// 4-component ASTC, 6x6x6 blocks, sRGB

		//
		// ATC
		//
		case GL_ATC_RGB_AMD:									return vk::Format::eUndefined;					// 3-component, 4x4 blocks, unsigned normalized
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:					return vk::Format::eUndefined;					// 4-component, 4x4 blocks, unsigned normalized
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:				return vk::Format::eUndefined;					// 4-component, 4x4 blocks, unsigned normalized

		//
		// Palletized
		//
		case GL_PALETTE4_RGB8_OES:								return vk::Format::eUndefined;					// 3-component 8:8:8,   4-bit palette, unsigned normalized
		case GL_PALETTE4_RGBA8_OES:								return vk::Format::eUndefined;					// 4-component 8:8:8:8, 4-bit palette, unsigned normalized
		case GL_PALETTE4_R5_G6_B5_OES:							return vk::Format::eUndefined;					// 3-component 5:6:5,   4-bit palette, unsigned normalized
		case GL_PALETTE4_RGBA4_OES:								return vk::Format::eUndefined;					// 4-component 4:4:4:4, 4-bit palette, unsigned normalized
		case GL_PALETTE4_RGB5_A1_OES:							return vk::Format::eUndefined;					// 4-component 5:5:5:1, 4-bit palette, unsigned normalized
		case GL_PALETTE8_RGB8_OES:								return vk::Format::eUndefined;					// 3-component 8:8:8,   8-bit palette, unsigned normalized
		case GL_PALETTE8_RGBA8_OES:								return vk::Format::eUndefined;					// 4-component 8:8:8:8, 8-bit palette, unsigned normalized
		case GL_PALETTE8_R5_G6_B5_OES:							return vk::Format::eUndefined;					// 3-component 5:6:5,   8-bit palette, unsigned normalized
		case GL_PALETTE8_RGBA4_OES:								return vk::Format::eUndefined;					// 4-component 4:4:4:4, 8-bit palette, unsigned normalized
		case GL_PALETTE8_RGB5_A1_OES:							return vk::Format::eUndefined;					// 4-component 5:5:5:1, 8-bit palette, unsigned normalized

		//
		// Depth/stencil
		//
		case GL_DEPTH_COMPONENT16:								return vk::Format::eD16Unorm;
		case GL_DEPTH_COMPONENT24:								return vk::Format::eX8D24UnormPack32;
		case GL_DEPTH_COMPONENT32:								return vk::Format::eUndefined;
		case GL_DEPTH_COMPONENT32F:								return vk::Format::eD32Sfloat;
		case GL_DEPTH_COMPONENT32F_NV:							return vk::Format::eD32Sfloat;
		case GL_STENCIL_INDEX1:									return vk::Format::eUndefined;
		case GL_STENCIL_INDEX4:									return vk::Format::eUndefined;
		case GL_STENCIL_INDEX8:									return vk::Format::eS8Uint;
		case GL_STENCIL_INDEX16:								return vk::Format::eUndefined;
		case GL_DEPTH24_STENCIL8:								return vk::Format::eD24UnormS8Uint;
		case GL_DEPTH32F_STENCIL8:								return vk::Format::eD32SfloatS8Uint;
		case GL_DEPTH32F_STENCIL8_NV:							return vk::Format::eD32SfloatS8Uint;

		//
		// Specific types
		//
		case 0x1406: 											return vk::Format::eR32Sfloat;
		case 0x8B50: 											return vk::Format::eR32G32Sfloat;
		case 0x8B51: 											return vk::Format::eR32G32B32Sfloat;
		case 0x8B52: 											return vk::Format::eR32G32B32A32Sfloat;
		case 0x1404:											return vk::Format::eR32Sint;
		case 0x8B53: 											return vk::Format::eR32G32Sint;
		case 0x8B54: 											return vk::Format::eR32G32B32Sint;
		case 0x8B55: 											return vk::Format::eR32G32B32A32Sint;
		case 0x1405: 											return vk::Format::eR32Sint;
		case 0x8DC6: 											return vk::Format::eR32G32Sint;
		case 0x8DC7: 											return vk::Format::eR32G32B32Sint;
		case 0x8DC8: 											return vk::Format::eR32G32B32A32Sint;

		default:												return vk::Format::eUndefined;
	}
}

bool VulkanStaticHelper::HasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}