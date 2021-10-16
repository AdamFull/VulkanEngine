#include "TextureCubemap.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void TextureCubemap::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void TextureCubemap::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        TextureBase::Update(imageIndex, pUniformBuffer);
    }

    void TextureCubemap::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        TextureBase::Bind(commandBuffer, imageIndex);
    }

    void TextureCubemap::Cleanup()
    {
        TextureBase::Cleanup();
    }

    void TextureCubemap::Destroy()
    {
        TextureBase::Destroy();
    }

    void TextureCubemap::LoadFromMemory(ktxTexture* info, vk::Format format)
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
        imageInfo.arrayLayers = info->numFaces;
        imageInfo.format = format;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
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
        barrier.subresourceRange.layerCount = info->numFaces;
        vBarriers.push_back(barrier);

        UDevice->TransitionImageLayout(image, vBarriers, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        std::vector<vk::BufferImageCopy> vRegions;
		for (uint32_t face = 0; face < info->numFaces; face++)
		{
			for (uint32_t level = 0; level < info->numLevels; level++)
			{
                ktx_size_t offset;
				KTX_error_code ret = ktxTexture_GetImageOffset(info, level, 0, face, &offset);
				assert(ret == KTX_SUCCESS);
                vk::BufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = info->numLayers;
				bufferCopyRegion.imageExtent.width = info->baseWidth >> level;
				bufferCopyRegion.imageExtent.height = info->baseHeight >> level;
				bufferCopyRegion.imageExtent.depth = info->baseDepth;
				bufferCopyRegion.bufferOffset = offset;
				vRegions.push_back(bufferCopyRegion);
            }
        }
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, vRegions);

        UDevice->TransitionImageLayout(image, vBarriers, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        //GenerateMipmaps(image, mipLevels, format, width, height, vk::ImageAspectFlagBits::eColor);
        //UDevice->TransitionImageLayout(image, mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);

        stagingBuffer.Destroy(UDevice);


        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::eCube;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = info->numLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = info->numFaces;
        view = UDevice->CreateImageView(image, viewInfo);

        UDevice->CreateSampler(sampler, mipLevels);
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        UpdateDescriptor();
    }
}