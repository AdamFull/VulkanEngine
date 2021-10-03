#include "VulkanTexture.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace Engine
{
    void TextureBase::Create(std::string srResourceName)
    {
        
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

        UDevice->Destroy(sampler);
        UDevice->Destroy(view);
        UDevice->Destroy(image);
        UDevice->Destroy(deviceMemory);
    }

    void TextureBase::UpdateDescriptor()
    {
        descriptor.sampler = sampler;
		descriptor.imageView = view;
		descriptor.imageLayout = imageLayout;
    }

    void TextureBase::GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, vk::Extent3D sizes, vk::ImageAspectFlags aspectFlags)
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

        int32_t mipWidth = sizes.width;
        int32_t mipHeight = sizes.height;

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
    
    /*******************************************************Texture2D*****************************************************************/
    void Texture2D::Create(std::string srResourceName)
    {
        Load(srResourceName);
    }

    void Texture2D::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void Texture2D::Update(uint32_t imageIndex)
    {
        TextureBase::Update(imageIndex);
    }

    void Texture2D::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        TextureBase::Bind(commandBuffer, imageIndex);
    }

    void Texture2D::Cleanup()
    {
        TextureBase::Cleanup();
    }

    void Texture2D::Destroy()
    {
        TextureBase::Destroy();
    }

    void Texture2D::Load(std::string srPath)
    {
        int w, h, c;
        unsigned char* raw_data = stbi_load(srPath.c_str(), &w, &h, &c, STBI_rgb_alpha);

        if (!raw_data)
        {
            //TODO: check result
        }

        width = static_cast<uint32_t>(w);
        height = static_cast<uint32_t>(h);
        channels = static_cast<uint32_t>(c);
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

        vk::DeviceSize imgSize = width * height * 4;
        auto compiledSize = vk::Extent3D{width, height, channels};

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(UDevice, imgSize, 1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(UDevice);
        stagingBuffer.Write(UDevice, raw_data);

        stbi_image_free(raw_data);

        UDevice->CreateImage(image, deviceMemory, compiledSize, mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, 
                    vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
                    vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        UDevice->TransitionImageLayout(image, mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        UDevice->CopyBufferToImage(stagingBuffer.GetBuffer(), image, compiledSize);
        GenerateMipmaps(image, mipLevels, vk::Format::eR8G8B8A8Srgb, compiledSize, vk::ImageAspectFlagBits::eColor);

        stagingBuffer.Destroy(UDevice);

        view = UDevice->CreateImageView(image, mipLevels, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        UDevice->CreateSampler(sampler, mipLevels);
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        UpdateDescriptor();
    }

    /*******************************************************TextureCubemap*****************************************************************/
    void TextureCubemap::Create(std::string srResourcePath)
    {
        TextureBase::Create(srResourcePath);
    }

    void TextureCubemap::ReCreate()
    {
        TextureBase::ReCreate();
    }

    void TextureCubemap::Update(uint32_t imageIndex)
    {
        TextureBase::Update(imageIndex);
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
}