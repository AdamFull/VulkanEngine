#include "VulkanTexture.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void TextureBase::ReCreate()
    {
        ResourceBase::ReCreate();
    }

    void TextureBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
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

        UDevice->Destroy(image);
        UDevice->Destroy(view);
        UDevice->Destroy(deviceMemory);
        UDevice->Destroy(sampler);
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
        UDevice->GetPhysical().getFormatProperties(format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) 
        {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        vk::CommandBuffer commandBuffer = UDevice->BeginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = width;
        int32_t mipHeight = height;

        for (uint32_t i = 1; i < mipLevels; i++) 
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, 
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            vk::ImageBlit blit{};
            blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = aspectFlags;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = aspectFlags;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            commandBuffer.blitImage(
                image, vk::ImageLayout::eTransferSrcOptimal,
                image, vk::ImageLayout::eTransferDstOptimal,
                1, &blit,
                vk::Filter::eLinear);

            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &barrier);

        UDevice->EndSingleTimeCommands(commandBuffer);
    }

    vk::ImageType TextureBase::TypeFromKtx(uint32_t type)
    {
        switch (type)
        {
        case 1: return vk::ImageType::e1D; break;
        case 2: return vk::ImageType::e2D; break;
        case 3: return vk::ImageType::e3D; break;
        }
    }

    void TextureBase::LoadFromFile(std::string srPath)
    {
        ktxTexture* texture;
        vk::Format format;
        FilesystemHelper::LoadImage(srPath.c_str(), &texture, &format);

        LoadFromMemory(texture, format);

        FilesystemHelper::CloseImage(&texture);
    }

    void TextureBase::LoadFromMemory(ktxTexture* info, vk::Format format)
    {
        width = info->baseWidth;
        height = info->baseHeight;
        channels = 4;
        mipLevels = info->numLevels;

        vk::DeviceSize imgSize = info->dataSize;

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(UDevice, imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(UDevice);
        stagingBuffer.Write(UDevice, (void*)info->pData);

        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = TypeFromKtx(info->numDimensions);
        imageInfo.extent.width = info->baseWidth;
        imageInfo.extent.height = info->baseHeight;
        imageInfo.extent.depth = info->baseDepth;
        imageInfo.mipLevels = info->numLevels;
        imageInfo.arrayLayers = info->numLayers;
        imageInfo.format = format;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.samples = vk::SampleCountFlagBits::e1;

        UDevice->CreateImage(image, deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

        std::vector<vk::ImageMemoryBarrier> vBarriers;
        vk::ImageMemoryBarrier barrier{};
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = info->numLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = info->numLayers;
        vBarriers.push_back(barrier);

        UDevice->TransitionImageLayout(image, vBarriers, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        std::vector<vk::BufferImageCopy> vRegions;
        vk::BufferImageCopy region = {};
		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = info->numLayers;
		region.imageExtent.width = info->baseWidth;
		region.imageExtent.height = info->baseHeight;
		region.imageExtent.depth = info->baseDepth;
		region.bufferOffset = 0;
        vRegions.push_back(region);
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);

        GenerateMipmaps(image, mipLevels, format, width, height, vk::ImageAspectFlagBits::eColor);
        //UDevice->TransitionImageLayout(image, mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);

        stagingBuffer.Destroy(UDevice);

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = info->numLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = info->numLayers;

        view = UDevice->CreateImageView(image, viewInfo);

        UDevice->CreateSampler(sampler, mipLevels);
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        UpdateDescriptor();
    }
}