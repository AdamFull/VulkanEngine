#include "Image.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

Image::~Image()
{
    UDevice->Destroy(image);
    UDevice->Destroy(view);
    UDevice->Destroy(deviceMemory);

    if(!bUsingInternalSampler)
        UDevice->Destroy(sampler);
}

void Image::ReCreate()
{

}

void Image::Update(uint32_t imageIndex)
{
    UpdateDescriptor();
}

void Image::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
}

void Image::Cleanup()
{
}

void Image::Destroy()
{
}

void Image::UpdateDescriptor()
{
    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;
}

void Image::SetAttachment(ETextureAttachmentType eAttachment)
{
    attachment = eAttachment;
}

ETextureAttachmentType Image::GetAttachment()
{
    return attachment;
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
    bUsingInternalSampler = true;
    sampler = internalSampler;
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
    fParams.format = format;

    uint32_t maxImageDimension3D(devprops.limits.maxImageDimension3D);
    if (info->baseWidth > maxImageDimension3D || info->baseHeight > maxImageDimension3D || info->baseDepth > maxImageDimension3D)
    {
        std::cout << "Error: Requested texture dimensions is greater than supported 3D texture dimension!" << std::endl;
        return;
    }

    fParams.width = info->baseWidth;
    fParams.height = info->baseHeight;
    fParams.depth = info->baseDepth;
    fParams.mipLevels = info->numLevels;
    fParams.layerCount = info->numLayers;

    vk::ImageCreateInfo imageInfo{};
    // Select image type
    if (info->baseDepth > 1)
        imageInfo.imageType = vk::ImageType::e3D;
    else
        imageInfo.imageType = TypeFromKtx(info->numDimensions);

    imageInfo.extent.width = info->baseWidth;
    imageInfo.extent.height = info->baseHeight;
    imageInfo.extent.depth = info->baseDepth;
    imageInfo.mipLevels = info->numLevels;

    if (info->isArray)
        imageInfo.arrayLayers = info->numLayers;
    else if (info->isCubemap)
        imageInfo.arrayLayers = info->numFaces;
    else
        imageInfo.arrayLayers = 1;

    fParams.instCount = imageInfo.arrayLayers;

    imageInfo.format = format;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = flags;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (info->isArray)
        imageInfo.flags = vk::ImageCreateFlagBits::e2DArrayCompatible;
    else if (info->isCubemap)
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
    else
        imageInfo.flags = vk::ImageCreateFlags{};

    imageInfo.samples = vk::SampleCountFlagBits::e1;

    UDevice->CreateImage(image, deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

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
    viewInfo.subresourceRange.levelCount = info->numLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = fParams.instCount;

    view = UDevice->CreateImageView(image, viewInfo);

    if(!sampler)
    {
        auto addressMode = info->isArray || info->isCubemap || info->baseDepth > 1 ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;
        UDevice->CreateSampler(sampler, fParams.mipLevels, addressMode);
    }
    //imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

void Image::TransitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips)
{
    vk::CommandBuffer commandBuffer = UDevice->BeginSingleTimeCommands();
    TransitionImageLayout(commandBuffer, imageLayout, newLayout, aspectFlags, use_mips);
    UDevice->EndSingleTimeCommands(commandBuffer);
}

void Image::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    TransitionImageLayout(commandBuffer, imageLayout, newLayout, aspectFlags, use_mips, base_mip);
}

void Image::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = base_mip;
    barrier.subresourceRange.levelCount = use_mips ? fParams.mipLevels : 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = fParams.instCount;
    vBarriers.push_back(barrier);
    
    UDevice->TransitionImageLayout(commandBuffer, image, vBarriers, oldLayout, newLayout);
    imageLayout = newLayout;
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

    commandBuffer.blitImage(image, imageLayout, image, dstLayout, 1, &blit, vk::Filter::eLinear);
}

void Image::CopyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<Image> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    commandBuffer.copyImage(image, imageLayout, m_pDst->image, dstLayout, 1, &region);
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
        region.imageSubresource.layerCount = fParams.instCount;
        region.imageExtent.width = info->baseWidth;
        region.imageExtent.height = info->baseHeight;
        region.imageExtent.depth = info->baseDepth;
        region.bufferOffset = 0;
        vRegions.push_back(region);
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);
        GenerateMipmaps(image, fParams.mipLevels, format, fParams.width, fParams.height, aspect);
    }
    else
    {
        std::vector<vk::BufferImageCopy> vRegions;
        for (uint32_t layer = 0; layer < fParams.instCount; layer++)
        {
            for (uint32_t level = 0; level < fParams.mipLevels; level++)
            {
                ktx_size_t offset;
                KTX_error_code ret = ktxTexture_GetImageOffset(info, level, 0, layer, &offset);
                assert(ret == KTX_SUCCESS);
                vk::BufferImageCopy region = {};
                region.imageSubresource.aspectMask = aspect;
                region.imageSubresource.mipLevel = level;
                region.imageSubresource.baseArrayLayer = layer;
                region.imageSubresource.layerCount = fParams.layerCount;
                region.imageExtent.width = info->baseWidth >> level;
                region.imageExtent.height = info->baseHeight >> level;
                region.imageExtent.depth = info->baseDepth;
                region.bufferOffset = offset;
                vRegions.push_back(region);
            }
        }
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);

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
    imageLayout = layout;
}

void Image::SetName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}