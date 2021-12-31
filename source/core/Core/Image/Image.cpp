#include "Image.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Loaders;

Image::~Image()
{
    UDevice->Destroy(m_image);
    UDevice->Destroy(m_view);
    UDevice->Destroy(m_deviceMemory);

    if(!m_bUsingInternalSampler)
        UDevice->Destroy(m_sampler);
}

void Image::UpdateDescriptor()
{
    m_descriptor.sampler = m_sampler;
    m_descriptor.imageView = m_view;
    m_descriptor.imageLayout = m_imageLayout;
}

void Image::GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags)
{
    vk::FormatProperties formatProperties;
    UDevice->GetPhysical().getFormatProperties(format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    vk::CommandBuffer commandBuffer = UDevice->BeginSingleTimeCommands();

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        TransitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, aspectFlags, false, i - 1);
        BlitImage(commandBuffer, vk::ImageLayout::eTransferDstOptimal, aspectFlags, i, mipWidth, mipHeight);
        TransitionImageLayout(commandBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, i - 1);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    TransitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, mipLevels - 1);

    UDevice->EndSingleTimeCommands(commandBuffer);
}

vk::ImageType Image::TypeFromKtx(uint32_t type)
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
}

void Image::LoadFromFile(std::string srPath)
{
    ktxTexture *texture;
    vk::Format format;
    ImageLoader::Load(srPath.c_str(), &texture, &format);

    LoadFromMemory(texture, format);

    ImageLoader::Close(&texture);
}

void Image::SetSampler(vk::Sampler& internalSampler)
{
    m_bUsingInternalSampler = true;
    m_sampler = internalSampler;
}

void Image::CreateEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t internalFormat, bool allocate_mem)
{
    vk::Format format;
    ktxTexture *texture;
    ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, width, height, depth, dims, internalFormat);

    InitializeTexture(texture, format);

    if(allocate_mem)
    {
        texture->pData = static_cast<uint8_t*>(calloc(texture->dataSize, sizeof(uint8_t)));
        WriteImageData(texture, format);
    }
    
    UpdateDescriptor();

    ImageLoader::Close(&texture);
}

void Image::InitializeTexture(ktxTexture *info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect)
{
    vk::PhysicalDeviceProperties devprops;
    UDevice->GetPhysical().getProperties(&devprops);
    m_format = format;

    uint32_t maxImageDimension3D(devprops.limits.maxImageDimension3D);
    if (info->baseWidth > maxImageDimension3D || info->baseHeight > maxImageDimension3D || info->baseDepth > maxImageDimension3D)
    {
        std::cout << "Error: Requested texture dimensions is greater than supported 3D texture dimension!" << std::endl;
        return;
    }

    m_extent = vk::Extent3D{info->baseWidth, info->baseHeight, info->baseDepth};
    m_mipLevels = info->numLevels;
    m_layerCount = info->numLayers;

    vk::ImageCreateInfo imageInfo{};
    // Select image type
    if (info->baseDepth > 1)
        imageInfo.imageType = vk::ImageType::e3D;
    else
        imageInfo.imageType = TypeFromKtx(info->numDimensions);

    imageInfo.extent = m_extent;
    imageInfo.mipLevels = m_mipLevels;

    if (info->isArray)
        imageInfo.arrayLayers = info->numLayers;
    else if (info->isCubemap)
        imageInfo.arrayLayers = info->numFaces;
    else
        imageInfo.arrayLayers = 1;

    m_instCount = imageInfo.arrayLayers;

    imageInfo.format = m_format;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = m_imageLayout;
    imageInfo.usage = flags;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (info->isArray)
        imageInfo.flags = vk::ImageCreateFlagBits::e2DArrayCompatible;
    else if (info->isCubemap)
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
    else
        imageInfo.flags = vk::ImageCreateFlags{};

    imageInfo.samples = vk::SampleCountFlagBits::e1;

    UDevice->CreateImage(m_image, m_deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

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
    viewInfo.subresourceRange.levelCount = m_mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = m_instCount;

    m_view = UDevice->CreateImageView(m_image, viewInfo);

    if(!m_sampler)
    {
        m_addressMode = info->isArray || info->isCubemap || info->baseDepth > 1 ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;
        UDevice->CreateSampler(m_sampler, m_mipLevels, m_addressMode, m_filter);
    }
    //imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

void Image::TransitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips)
{
    vk::CommandBuffer commandBuffer = UDevice->BeginSingleTimeCommands();
    TransitionImageLayout(commandBuffer, m_imageLayout, newLayout, aspectFlags, use_mips);
    UDevice->EndSingleTimeCommands(commandBuffer);
}

void Image::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    TransitionImageLayout(commandBuffer, m_imageLayout, newLayout, aspectFlags, use_mips, base_mip);
}

void Image::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = base_mip;
    barrier.subresourceRange.levelCount = use_mips ? m_mipLevels : 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = m_instCount;
    vBarriers.push_back(barrier);
    
    UDevice->TransitionImageLayout(commandBuffer, m_image, vBarriers, oldLayout, newLayout);
    m_imageLayout = newLayout;
}

void Image::BlitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight)
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

    commandBuffer.blitImage(m_image, m_imageLayout, m_image, dstLayout, 1, &blit, vk::Filter::eLinear);
}

void Image::CopyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<Image> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    commandBuffer.copyImage(m_image, m_imageLayout, m_pDst->m_image, dstLayout, 1, &region);
}

void Image::WriteImageData(ktxTexture *info, vk::Format format, vk::ImageAspectFlags aspect)
{
    vk::DeviceSize imgSize = info->dataSize;

    Core::VulkanBuffer stagingBuffer;
    stagingBuffer.Create(imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto result = stagingBuffer.MapMem();
    stagingBuffer.Write((void *)info->pData);

    TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal, aspect);

    if (info->generateMipmaps)
    {
        std::vector<vk::BufferImageCopy> vRegions;
        vk::BufferImageCopy region = {};
        region.imageSubresource.aspectMask = aspect;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = m_instCount;
        region.imageExtent.width = info->baseWidth;
        region.imageExtent.height = info->baseHeight;
        region.imageExtent.depth = info->baseDepth;
        region.bufferOffset = 0;
        vRegions.push_back(region);
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), m_image, vRegions);
        GenerateMipmaps(m_image, m_mipLevels, format, m_extent.width, m_extent.height, aspect);
    }
    else
    {
        std::vector<vk::BufferImageCopy> vRegions;
        for (uint32_t layer = 0; layer < m_instCount; layer++)
        {
            for (uint32_t level = 0; level < m_mipLevels; level++)
            {
                ktx_size_t offset;
                KTX_error_code ret = ktxTexture_GetImageOffset(info, level, 0, layer, &offset);
                assert(ret == KTX_SUCCESS);
                vk::BufferImageCopy region = {};
                region.imageSubresource.aspectMask = aspect;
                region.imageSubresource.mipLevel = level;
                region.imageSubresource.baseArrayLayer = layer;
                region.imageSubresource.layerCount = m_layerCount;
                region.imageExtent.width = info->baseWidth >> level;
                region.imageExtent.height = info->baseHeight >> level;
                region.imageExtent.depth = info->baseDepth;
                region.bufferOffset = offset;
                vRegions.push_back(region);
            }
        }
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), m_image, vRegions);

        TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal, aspect);
    }
}

void Image::LoadFromMemory(ktxTexture *info, vk::Format format)
{
    InitializeTexture(info, format);
    WriteImageData(info, format);
    UpdateDescriptor();
}

void Image::SetImageLayout(vk::ImageLayout layout)
{
    m_imageLayout = layout;
}