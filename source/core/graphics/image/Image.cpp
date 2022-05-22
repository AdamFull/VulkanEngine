#include "Image.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/editor/imgui_impl_vulkan.h"

using namespace Engine::Core;
using namespace Engine::Core::Loaders;

//Maybe add to image generation of attachment description and 

CImage::~CImage()
{
    CDevice::inst()->destroy(&_image);
    CDevice::inst()->destroy(&_view);
    CDevice::inst()->destroy(&_deviceMemory);

    if(!_bUsingInternalSampler)
        CDevice::inst()->destroy(&_sampler);
}

void CImage::updateDescriptor()
{
    _descriptor.sampler = _sampler;
    _descriptor.imageView = _view;
    _descriptor.imageLayout = _imageLayout;
}

void CImage::create(const std::string& srPath, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, 
vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    ktxTexture *texture;
    vk::Format format;
    CImageLoader::load(srPath.c_str(), &texture, &format);

    loadFromMemory(texture, format, flags, aspect, addressMode, filter);

    CImageLoader::close(&texture);
}

void CImage::generateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags)
{
    auto& physicalDevice = CDevice::inst()->getPhysical();
    assert(physicalDevice && "Trying to generate mipmaps, but physical device is invalid.");
    vk::FormatProperties formatProperties;
    physicalDevice.getFormatProperties(format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, aspectFlags, false, i - 1);
        blitImage(commandBuffer, vk::ImageLayout::eTransferDstOptimal, aspectFlags, i, mipWidth, mipHeight);
        transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, i - 1);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, mipLevels - 1);

    cmdBuf.submitIdle();
}

vk::ImageType CImage::typeFromKtx(uint32_t type)
{
    switch (type)
    {
    case 1:
        return vk::ImageType::e1D;
        break;
    case 2:
        return vk::ImageType::e2D;
        break;
    case 3:
        return vk::ImageType::e3D;
        break;
    }
    return vk::ImageType{};
}

void CImage::setSampler(vk::Sampler& internalSampler)
{
    _bUsingInternalSampler = true;
    _sampler = internalSampler;
}

void CImage::initializeTexture(ktxTexture *info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, 
vk::Filter filter, vk::SampleCountFlagBits samples)
{
    _format = format;
    _addressMode = addressMode;
    _filter = filter;
    _samples = samples;
    _usage = flags;

    //TODO: Add checking for texture type here
    if(!isSupportedDimension(info))
        throw std::runtime_error("Unsupported texture dimension.");

    _extent = vk::Extent3D{info->baseWidth, info->baseHeight, info->baseDepth};
    _mipLevels = info->numLevels;
    _layerCount = info->numLayers;

    vk::ImageCreateInfo imageInfo{};
    // Select image type
    if (info->baseDepth > 1)
        imageInfo.imageType = vk::ImageType::e3D;
    else
        imageInfo.imageType = typeFromKtx(info->numDimensions);

    imageInfo.extent = _extent;
    imageInfo.mipLevels = _mipLevels;

    if (info->isArray)
        imageInfo.arrayLayers = info->numLayers;
    else if (info->isCubemap)
        imageInfo.arrayLayers = 6;
    else
        imageInfo.arrayLayers = 1;

    _instCount = imageInfo.arrayLayers;

    imageInfo.format = _format;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = _imageLayout;
    imageInfo.usage = flags;
    imageInfo.samples = _samples;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (info->isArray)
        imageInfo.flags = vk::ImageCreateFlagBits::e2DArrayCompatible;
    else if (info->isCubemap)
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
    else
        imageInfo.flags = vk::ImageCreateFlags{};

    imageInfo.samples = CDevice::inst()->getSamples();

    createImage(_image, _deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageViewCreateInfo viewInfo{};
    if (info->isArray)
        viewInfo.viewType = vk::ImageViewType::e2DArray;
    else if (info->isCubemap)
        viewInfo.viewType = vk::ImageViewType::eCube;
    else if (info->baseDepth > 1)
        viewInfo.viewType = vk::ImageViewType::e3D;
    else
        viewInfo.viewType = vk::ImageViewType::e2D;

    viewInfo.format = format;
    viewInfo.components = {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA};
    viewInfo.subresourceRange.aspectMask = aspect;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = _mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = _instCount;
    viewInfo.image = _image;

    vk::Result res = CDevice::inst()->create(viewInfo, &_view);
    assert(res == vk::Result::eSuccess && "Cannot create image view");

    if(!_sampler)
    {
        //_addressMode = info->isArray || info->isCubemap || info->baseDepth > 1 ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;
        createSampler(_sampler, _mipLevels, _addressMode, _filter);
    }
}

vk::Format CImage::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    auto& physicalDevice = CDevice::inst()->getPhysical();
    assert(physicalDevice && "Trying to find supported format, but physical device is invalid.");
    for (vk::Format format : candidates)
    {
        vk::FormatProperties props;
        physicalDevice.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    // TODO: Handle null result
    throw std::runtime_error("Failed to find supported format!");
}

vk::Format CImage::getDepthFormat()
{
    return findSupportedFormat
    (
        {
            vk::Format::eD32Sfloat, 
            vk::Format::eD32SfloatS8Uint, 
            vk::Format::eD24UnormS8Uint
        },
        vk::ImageTiling::eOptimal, 
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void CImage::createImage(vk::Image &image, vk::DeviceMemory &memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties)
{
    vk::Result res;
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to create image, byt logical device is not valid.");

    res = CDevice::inst()->create(createInfo, &image);
    assert(res == vk::Result::eSuccess && "Image was not created");

    vk::MemoryRequirements memReq{};
    vkDevice.getImageMemoryRequirements(image, &memReq);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = CDevice::inst()->findMemoryType(memReq.memoryTypeBits, properties);

    res = CDevice::inst()->create(allocInfo, &memory);
    assert(res == vk::Result::eSuccess && "Image memory was not allocated");

    vkDevice.bindImageMemory(image, memory, 0);
}

void CImage::transitionImageLayout(vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    transitionImageLayout(commandBuffer, image, vBarriers, oldLayout, newLayout);

    cmdBuf.submitIdle();
}

void CImage::transitionImageLayout(vk::CommandBuffer& internalBuffer, vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    for (auto &barrier : vBarriers)
    {
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eGeneral)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::eTransferSrcOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferSrcOptimal && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eTransferSrcOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eTransferSrcOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }
    }

    internalBuffer.pipelineBarrier(
        sourceStage,
        destinationStage,
        vk::DependencyFlags(),
        0, nullptr, 0, nullptr,
        static_cast<uint32_t>(vBarriers.size()), vBarriers.data());
}

void CImage::copyBufferToImage(vk::Buffer &buffer, vk::Image &image, std::vector<vk::BufferImageCopy> vRegions)
{
    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, static_cast<uint32_t>(vRegions.size()), vRegions.data());
    cmdBuf.submitIdle();
}

void CImage::copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region)
{
    commandBuffer.copyImage(src, srcLayout, dst, dstLayout, 1, &region);
}

void CImage::createSampler(vk::Sampler &sampler, uint32_t mip_levels, vk::SamplerAddressMode eAddressMode, vk::Filter magFilter)
{
    auto& physicalDevice = CDevice::inst()->getPhysical();
    assert(physicalDevice && "Trying to create sampler, but physical device is invalid.");
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = eAddressMode;
    samplerInfo.addressModeV = eAddressMode;
    samplerInfo.addressModeW = eAddressMode;

    vk::PhysicalDeviceProperties properties{};
    properties = physicalDevice.getProperties();

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mip_levels);

    // TODO: Result handle
   vk::Result res = CDevice::inst()->create(samplerInfo, &sampler);
    assert(res == vk::Result::eSuccess && "Texture sampler was not created");
}

bool CImage::isSupportedDimension(ktxTexture *info)
{
    auto& physicalDevice = CDevice::inst()->getPhysical();
    assert(physicalDevice && "Trying to check supported dibension, but physical device is invalid.");
    vk::PhysicalDeviceProperties devprops;
    physicalDevice.getProperties(&devprops);

    if(info->isCubemap)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimensionCube || 
        info->baseHeight > devprops.limits.maxImageDimensionCube || 
        info->baseDepth > devprops.limits.maxImageDimensionCube);
    }

    if(info->isArray)
    {
        return !(info->numLayers > devprops.limits.maxImageArrayLayers || 
        info->numFaces > devprops.limits.maxImageArrayLayers);
    }

    if(info->numDimensions == 1)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension1D ||
        info->baseHeight > devprops.limits.maxImageDimension1D ||
        info->baseDepth > devprops.limits.maxImageDimension1D);
    }

    if(info->numDimensions == 2)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension2D ||
        info->baseHeight > devprops.limits.maxImageDimension2D ||
        info->baseDepth > devprops.limits.maxImageDimension2D); 
    }

    if(info->numDimensions == 3)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension3D ||
        info->baseHeight > devprops.limits.maxImageDimension3D ||
        info->baseDepth > devprops.limits.maxImageDimension3D); 
    }

    return true;
}

void CImage::transitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips)
{
    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    transitionImageLayout(commandBuffer, _imageLayout, newLayout, aspectFlags, use_mips);
    cmdBuf.submitIdle();
}

void CImage::transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    transitionImageLayout(commandBuffer, _imageLayout, newLayout, aspectFlags, use_mips, base_mip);
}

void CImage::transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = base_mip;
    barrier.subresourceRange.levelCount = use_mips ? _mipLevels : 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _instCount;
    vBarriers.push_back(barrier);
    
    transitionImageLayout(commandBuffer, _image, vBarriers, oldLayout, newLayout);
    _imageLayout = newLayout;
}

void CImage::blitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight)
{
    vk::ImageBlit blit{};
    blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
    blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = aspectFlags;
    blit.srcSubresource.mipLevel = level - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
    blit.dstOffsets[1] = vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = aspectFlags;
    blit.dstSubresource.mipLevel = level;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    commandBuffer.blitImage(_image, _imageLayout, _image, dstLayout, 1, &blit, vk::Filter::eLinear);
}

void CImage::copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    copyTo(commandBuffer, _image, m_pDst->_image, _imageLayout, dstLayout, region);
}

void CImage::writeImageData(ktxTexture *info, vk::Format format, vk::ImageAspectFlags aspect)
{
    vk::DeviceSize imgSize = info->dataSize;

    Core::CVulkanBuffer stagingBuffer;
    stagingBuffer.create(imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto result = stagingBuffer.mapMem();
    stagingBuffer.write((void *)info->pData);

    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, aspect);

    if (info->generateMipmaps)
    {
        std::vector<vk::BufferImageCopy> vRegions;
        vk::BufferImageCopy region = {};
        region.imageSubresource.aspectMask = aspect;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = _instCount;
        region.imageExtent.width = info->baseWidth;
        region.imageExtent.height = info->baseHeight;
        region.imageExtent.depth = info->baseDepth;
        region.bufferOffset = 0;
        vRegions.push_back(region);

        auto buffer = stagingBuffer.getBuffer();
        copyBufferToImage(buffer, _image, vRegions);
        generateMipmaps(_image, _mipLevels, format, _extent.width, _extent.height, aspect);
    }
    else
    {
        std::vector<vk::BufferImageCopy> vRegions;
        for (uint32_t layer = 0; layer < _instCount; layer++)
        {
            for (uint32_t level = 0; level < _mipLevels; level++)
            {
                ktx_size_t offset;
                KTX_error_code ret = ktxTexture_GetImageOffset(info, level, 0, layer, &offset);
                assert(ret == KTX_SUCCESS);
                vk::BufferImageCopy region = {};
                region.imageSubresource.aspectMask = aspect;
                region.imageSubresource.mipLevel = level;
                region.imageSubresource.baseArrayLayer = layer;
                region.imageSubresource.layerCount = _layerCount;
                region.imageExtent.width = info->baseWidth >> level;
                region.imageExtent.height = info->baseHeight >> level;
                region.imageExtent.depth = info->baseDepth;
                region.bufferOffset = offset;
                vRegions.push_back(region);
            }
        }
        auto buffer = stagingBuffer.getBuffer();
        copyBufferToImage(buffer, _image, vRegions);

        transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal, aspect);
    }
}

void CImage::loadFromMemory(ktxTexture *info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, 
vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    initializeTexture(info, format, flags, aspect, addressMode, filter, vk::SampleCountFlagBits::e1);
    writeImageData(info, format, aspect);
    updateDescriptor();
}

void CImage::setImageLayout(vk::ImageLayout layout)
{
    _imageLayout = layout;
}

vk::DescriptorImageInfo& CImage::getDescriptor()
{
    if(_imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal && (_usage & vk::ImageUsageFlagBits::eInputAttachment))
    {
        _imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        updateDescriptor();
    }
    return _descriptor;
}

vk::DescriptorSet& CImage::getDescriptorSet()
{
    if(!descriptorSet)
    {
        descriptorSet = ImGui_ImplVulkan_AddTexture(_descriptor.sampler, _descriptor.imageView, _descriptor.imageLayout);
    }
    return descriptorSet;
}