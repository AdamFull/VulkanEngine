#include "Image2D.h"
#include "graphics/VulkanStaticHelper.h"

using namespace engine::core;

CImage2D::CImage2D(const fs::path& srPath, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    CImage::create(srPath, flags, aspect, addressMode, filter);
}

CImage2D::CImage2D(const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, 
vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, 
bool instantLayoutTransition, bool anisotropic, bool mipmaps)
{
    create(extent, format, layout, usage, aspect, filter, addressMode, samples, instantLayoutTransition, anisotropic, mipmaps);
}

void CImage2D::create(const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, 
vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, 
bool instantLayoutTransition, bool anisotropic, bool mipmaps)
{
    enableAnisotropy = anisotropic;
    utl::scope_ptr<FImageCreateInfo> texture = utl::make_scope<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = 1;
    texture->numDimensions = 2;
    texture->generateMipmaps = mipmaps;
    texture->numLevels = mipmaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1 : 1;
    texture->isArray = false;
    texture->numLayers = 1;
    texture->numFaces = 1;
    texture->dataSize = extent.width * extent.height * (texture->baseDepth > 1 ? texture->baseDepth : 4);

    initializeTexture(texture, format, usage, aspect, addressMode, filter, samples);

    if(instantLayoutTransition)
        transitionImageLayout(layout, aspect, mipmaps);
    else
        setImageLayout(layout);
    updateDescriptor();
}
