#pragma once
#include "external/ktx/include/ktx.h"
#include "external/ktx/lib/vk_format.h"

namespace Engine
{
    namespace Core
    {
        namespace Loaders
        {
            /**
             * @brief Class helper for loading images with ktx and stb library
             * 
             */
            struct CImageLoader
            {
            public:
                /**
                 * @brief Loads texture file
                 * 
                 * @brief Loads image with stb library formats
                 * 
                 * @param filename Local file path in assets
                 * @param target Khronos texture object
                 * @param format Vulkan texture format
                 * @return true when texture was loaded successfully
                 */
                static bool load(char const *filename, ktxTexture **target, vk::Format *format);

                /**
                 * @brief Closes texture file
                 * 
                 * @param target Khronos texture object
                 */
                static void close(ktxTexture **target);

                /**
                 * @brief Allocates Khronos texture with data
                 * 
                 * @param data Pointer to data
                 * @param target Khronos texture object
                 * @param format Vulkan format
                 * @param width Texture width
                 * @param height Texture height
                 * @param depth Texture depth
                 * @param dims Texture dimensiuns
                 * @param overrideFormat Override default texture format
                 * @param calcMips Use mipmap calculation
                 * @return true when texture was loaded successfully
                 */
                static bool allocateRawDataAsKTXTexture(unsigned char *data, ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);

                /**
                 * @brief Allocates empty Khronos texture object as cubemap
                 * 
                 * @param target Khronos texture object
                 * @param format Vulkan format
                 * @param width Texture width
                 * @param height Texture height
                 * @param depth Texture depth
                 * @param dims Texture dimensiuns
                 * @param overrideFormat Override default texture format
                 * @param calcMips Use mipmap calculation
                 * @return true when texture was loaded successfully
                 */
                static bool allocateRawDataAsKTXTextureCubemap(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);

                /**
                 * @brief Allocates empty Khronos texture object
                 * 
                 * @param target Khronos texture object
                 * @param format Vulkan format
                 * @param width Texture width
                 * @param height Texture height
                 * @param depth Texture depth
                 * @param dims Texture dimensiuns
                 * @param overrideFormat Override default texture format
                 * @param calcMips Use mipmap calculation
                 * @return true when texture was loaded successfully
                 */
                static bool allocateRawDataAsKTXTexture(ktxTexture **target, vk::Format *format, uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t overrideFormat = GL_SRGB8_ALPHA8, bool calcMips = false);

            private:
                /**
                 * @brief Loads image with stb library formats
                 * 
                 * @param filename Local file path in assets
                 * @param target Khronos texture object
                 * @param format Vulkan texture format
                 * @return true when texture was loaded successfully
                 */
                static bool loadSTB(char const *filename, ktxTexture **target, vk::Format *format);

                /**
                 * @brief Loads image with ktx library format
                 * 
                 * @param filename Local file path in assets
                 * @param target Khronos texture object
                 * @param format Vulkan texture format
                 * @return true when texture was loaded successfully
                 */
                static bool loadKTX(char const *filename, ktxTexture **target, vk::Format *format);
            };
        }
    }

}