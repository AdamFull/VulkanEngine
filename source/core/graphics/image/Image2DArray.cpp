#include "Image2DArray.h"
#include "graphics/VulkanStaticHelper.h"

using namespace engine::core;

void CImage2DArray::create(uint32_t layers, const vk::Extent2D &extent, vk::Format format, vk::ImageLayout layout,
						vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect, vk::Filter filter, 
                        vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples,
						bool instantLayoutTransition, bool anisotropic, bool mipmaps)
{
    scope_ptr<FImageCreateInfo> texture = make_scope<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = 1;
    texture->numDimensions = 2;
    texture->generateMipmaps = mipmaps;
    texture->numLevels = mipmaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1 : 1;
    texture->isArray = true;
    texture->numLayers = layers;
    texture->numFaces = 1;
    texture->dataSize = (extent.width * extent.height * (texture->baseDepth > 1 ? texture->baseDepth : 4) * texture->numLayers);

    initializeTexture(texture, format, usage, aspect, addressMode, filter, samples);

    if(instantLayoutTransition)
        transitionImageLayout(layout, aspect, mipmaps);
    else
        setImageLayout(layout);
    updateDescriptor();
}
