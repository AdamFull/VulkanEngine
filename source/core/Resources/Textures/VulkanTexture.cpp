#include "VulkanTexture.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanBuffer.h"
#include "Core/VulkanDevice.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

TextureBase::TextureBase(std::shared_ptr<Core::Device> device) :
m_device(device)
{
    
}

TextureBase::~TextureBase()
{
    m_device->Destroy(image);
    m_device->Destroy(view);
    m_device->Destroy(deviceMemory);
    m_device->Destroy(sampler);
}

void TextureBase::ReCreate()
{
    ResourceBase::ReCreate();
}

void TextureBase::Update(uint32_t imageIndex)
{
    ResourceBase::Update(imageIndex);
    UpdateDescriptor();
}

void TextureBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    ResourceBase::Bind(commandBuffer, imageIndex);
}

void TextureBase::Cleanup()
{
    ResourceBase::Cleanup();
}

void TextureBase::Destroy()
{
    ResourceBase::Destroy();
}

void TextureBase::UpdateDescriptor()
{
    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;
}

void TextureBase::SetAttachment(ETextureAttachmentType eAttachment)
{
    attachment = eAttachment;
}

ETextureAttachmentType TextureBase::GetAttachment()
{
    return attachment;
}

void TextureBase::GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags)
{
    vk::FormatProperties formatProperties;
    m_device->GetPhysical().getFormatProperties(format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    vk::CommandBuffer commandBuffer = m_device->BeginSingleTimeCommands();

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

    m_device->EndSingleTimeCommands(commandBuffer);
}

vk::ImageType TextureBase::TypeFromKtx(uint32_t type)
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

void TextureBase::LoadFromFile(std::string srPath)
{
    ktxTexture *texture;
    vk::Format format;
    ImageLoader::Load(srPath.c_str(), &texture, &format);

    LoadFromMemory(texture, format);

    ImageLoader::Close(&texture);
}

void TextureBase::CreateEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t internalFormat, bool allocate_mem)
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

void TextureBase::InitializeTexture(ktxTexture *info, vk::Format format, vk::ImageUsageFlags flags)
{
    vk::PhysicalDeviceProperties devprops;
    m_device->GetPhysical().getProperties(&devprops);

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

    m_device->CreateImage(image, deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

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
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = info->numLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = fParams.instCount;

    view = m_device->CreateImageView(image, viewInfo);

    auto addressMode = info->isArray || info->isCubemap || info->baseDepth > 1 ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;
    m_device->CreateSampler(sampler, fParams.mipLevels, addressMode);
    //imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

void TextureBase::TransitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips)
{
    vk::CommandBuffer commandBuffer = m_device->BeginSingleTimeCommands();
    TransitionImageLayout(commandBuffer, imageLayout, newLayout, aspectFlags, use_mips);
    m_device->EndSingleTimeCommands(commandBuffer);
}

void TextureBase::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    TransitionImageLayout(commandBuffer, imageLayout, newLayout, aspectFlags, use_mips, base_mip);
}

void TextureBase::TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
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
    
    m_device->TransitionImageLayout(commandBuffer, image, vBarriers, oldLayout, newLayout);
    imageLayout = newLayout;
}

void TextureBase::BlitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight)
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

void TextureBase::CopyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<TextureBase> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    commandBuffer.copyImage(image, imageLayout, m_pDst->image, dstLayout, 1, &region);
}

void TextureBase::WriteImageData(ktxTexture *info, vk::Format format)
{
    vk::DeviceSize imgSize = info->dataSize;

    Core::VulkanBuffer stagingBuffer(m_device);
    stagingBuffer.Create(imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto result = stagingBuffer.MapMem();
    stagingBuffer.Write((void *)info->pData);

    TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor);

    if (info->generateMipmaps)
    {
        std::vector<vk::BufferImageCopy> vRegions;
        vk::BufferImageCopy region = {};
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = fParams.instCount;
        region.imageExtent.width = info->baseWidth;
        region.imageExtent.height = info->baseHeight;
        region.imageExtent.depth = info->baseDepth;
        region.bufferOffset = 0;
        vRegions.push_back(region);
        m_device->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);
        GenerateMipmaps(image, fParams.mipLevels, format, fParams.width, fParams.height, vk::ImageAspectFlagBits::eColor);
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
                region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
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
        m_device->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);

        TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor);
    }
}

void TextureBase::LoadFromMemory(ktxTexture *info, vk::Format format)
{
    InitializeTexture(info, format);
    WriteImageData(info, format);
    UpdateDescriptor();
}