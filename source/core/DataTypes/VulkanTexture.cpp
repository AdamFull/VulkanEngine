#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace Engine
{
    void VulkanTextureBase::UpdateDescriptor()
    {
        descriptor.sampler = sampler;
		descriptor.imageView = view;
		descriptor.imageLayout = imageLayout;
    }

    void VulkanTextureBase::Destroy(vk::UniqueDevice& device)
    {
        device->destroySampler(sampler);
        device->destroyImageView(view);
        device->destroyImage(image);
        device->freeMemory(deviceMemory);
    }

    void VulkanTexture2D::LoadTextureFromFile(std::string srPath, vk::ImageLayout layout, vk::UniqueDevice& device)
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

        auto& render = Engine::PEngine()->PVulkan();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        render->CreateOnDeviceBuffer(imgSize, vk::BufferUsageFlagBits::eTransferSrc,
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                             stagingBuffer,
                             stagingBufferMemory);

        render->MoveToMemory(raw_data, stagingBufferMemory , imgSize);

        stbi_image_free(raw_data);

        render->CreateImage(image, deviceMemory, compiledSize, mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, 
                    vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
                    vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        render->TransitionImageLayout(image, mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        render->CopyBufferToImage(stagingBuffer, image, compiledSize);
        //render->TransitionImageLayout(image, mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        render->GenerateMipmaps(image, mipLevels, vk::Format::eR8G8B8A8Srgb, compiledSize, vk::ImageAspectFlagBits::eColor);

        device->destroyBuffer(stagingBuffer);
        device->freeMemory(stagingBufferMemory);

        view = render->CreateImageView(image, mipLevels, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        render->CreateSampler(sampler, mipLevels);
        imageLayout = layout;

        UpdateDescriptor();
    }
}