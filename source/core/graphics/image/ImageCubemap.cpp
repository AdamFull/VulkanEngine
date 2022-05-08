#include "ImageCubemap.h"
#include "graphics/VulkanStaticHelper.h"

using namespace Engine::Core;
using namespace Engine::Core::Loaders;

void CImageCubemap::create(const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, 
vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, 
bool instantLayoutTransition, bool anisotropic, bool mipmaps)
{
    ktxTexture *texture;
    vk::Format imageFormat;
    CImageLoader::allocateRawDataAsKTXTexture(&texture, &imageFormat, extent.width, extent.height, 1, 2, VulkanStaticHelper::vkFormatToGLFormat(format), mipmaps);
    texture->isCubemap = true;
    initializeTexture(texture, imageFormat, usage, aspect, addressMode, filter, samples);
    CImageLoader::close(&texture);
    if(instantLayoutTransition)
        transitionImageLayout(layout, aspect, mipmaps);
    else
        setImageLayout(layout);
    updateDescriptor();
}