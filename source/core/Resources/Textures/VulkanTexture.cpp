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
        ImageLoader::LoadImage(srPath.c_str(), &texture, &format);

        LoadFromMemory(texture, format);

        ImageLoader::CloseImage(&texture);
    }

    void TextureBase::CreateEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims)
    {
        vk::Format format;
        ktxTexture* texture;
        ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, width, height, depth, dims, GetInternalFormat());

        InitializeTexture(texture, format);

        ImageLoader::CloseImage(&texture);
    }

    void TextureBase::InitializeTexture(ktxTexture* info, vk::Format format)
    {
        vk::PhysicalDeviceProperties devprops;
        UDevice->GetPhysical().getProperties(&devprops);

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
        //Select image type
        if(info->baseDepth > 1)
            imageInfo.imageType = vk::ImageType::e3D;
        else
            imageInfo.imageType = TypeFromKtx(info->numDimensions);

        imageInfo.extent.width = info->baseWidth;
        imageInfo.extent.height = info->baseHeight;
        imageInfo.extent.depth = info->baseDepth;
        imageInfo.mipLevels = info->numLevels;

        if(info->isArray)
            imageInfo.arrayLayers = info->numLayers;
        else if(info->isCubemap)
            imageInfo.arrayLayers = info->numFaces;
        else
            imageInfo.arrayLayers = 1;

        fParams.instCount = imageInfo.arrayLayers;

        imageInfo.format = format;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;

        if(info->isArray)
            imageInfo.flags = vk::ImageCreateFlagBits::e2DArrayCompatible;
        else if(info->isCubemap)
            imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
        else
            imageInfo.flags = vk::ImageCreateFlags{};

        imageInfo.samples = vk::SampleCountFlagBits::e1;

        UDevice->CreateImage(image, deviceMemory, imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

        vk::ImageViewCreateInfo viewInfo{};
        if(info->isArray)
            viewInfo.viewType = vk::ImageViewType::e2DArray;
        else if(info->isCubemap)
            viewInfo.viewType = vk::ImageViewType::eCube;
        else if(info->baseDepth > 1)
            viewInfo.viewType = vk::ImageViewType::e3D;
        else 
            viewInfo.viewType = vk::ImageViewType::e2D;

        viewInfo.format = format;
        viewInfo.components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = info->numLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = fParams.instCount;

        view = UDevice->CreateImageView(image, viewInfo);

        auto addressMode = info->isArray || info->isCubemap || info->baseDepth > 1 ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;
        UDevice->CreateSampler(sampler, fParams.mipLevels, addressMode);
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    void TextureBase::WriteImageData(ktxTexture* info, vk::Format format)
    {
        vk::DeviceSize imgSize = info->dataSize;

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(UDevice, imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        auto result = stagingBuffer.MapMem(UDevice);
        stagingBuffer.Write(UDevice, (void*)info->pData);

        std::vector<vk::ImageMemoryBarrier> vBarriers;
        vk::ImageMemoryBarrier barrier{};
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = fParams.mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = fParams.instCount;
        vBarriers.push_back(barrier);

        UDevice->TransitionImageLayout(image, vBarriers, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        if(info->generateMipmaps)
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
            UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);
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
            UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);

            UDevice->TransitionImageLayout(image, vBarriers, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        stagingBuffer.Destroy(UDevice);
    }

    void TextureBase::LoadFromMemory(ktxTexture* info, vk::Format format)
    {
        InitializeTexture(info, format);
        WriteImageData(info, format);
        UpdateDescriptor();
    }
}