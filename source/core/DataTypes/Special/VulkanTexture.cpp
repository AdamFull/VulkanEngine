#include "VulkanTexture.h"
#include "VulkanMainStructures.h"
#include "VulkanDevice.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace Engine::Types::Texture
{
    void UpdateDescriptor(Base2D& texture)
    {
        texture.descriptor.sampler = texture.sampler;
		texture.descriptor.imageView = texture.info.view;
		texture.descriptor.imageLayout = texture.imageLayout;
    }

    void Destroy(Main::FVulkanEngine& engine, Base2D& texture)
    {
        engine.device.logical->destroySampler(texture.sampler);
        engine.device.logical->destroyImageView(texture.info.view);
        engine.device.logical->destroyImage(texture.info.image);
        engine.device.logical->freeMemory(texture.info.memory);
    }

    void Load(Main::FVulkanEngine& engine, Base2D& texture, std::string srTexturePath, vk::ImageLayout layout)
    {
        int w, h, c;
        unsigned char* raw_data = stbi_load(srTexturePath.c_str(), &w, &h, &c, STBI_rgb_alpha);

        if (!raw_data)
        {
            //TODO: check result
        }

        texture.width = static_cast<uint32_t>(w);
        texture.height = static_cast<uint32_t>(h);
        texture.channels = static_cast<uint32_t>(c);
        texture. mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture.width, texture.height)))) + 1;

        vk::DeviceSize imgSize = texture.width * texture.height * 4;

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        Device::CreateOnDeviceBuffer(engine, imgSize, vk::BufferUsageFlagBits::eTransferSrc,
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                             stagingBuffer,
                             stagingBufferMemory);

        Device::MoveToMemory(engine, raw_data, stagingBufferMemory , imgSize);

        stbi_image_free(raw_data);

        Device::CreateImage(engine, texture.info.image, texture.info.memory, texture.width, texture.height, texture.mipLevels, 
        vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | 
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        Device::TransitionImageLayout(engine, texture.info.image, texture.mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        Device::CopyBufferToImage(engine, stagingBuffer, texture.info.image, texture.width, texture.height, 1);
        Device::GenerateMipmaps(engine, texture.info.image, texture.mipLevels, vk::Format::eR8G8B8A8Srgb,  texture.width, texture.height, vk::ImageAspectFlagBits::eColor);

        engine.device.logical->destroyBuffer(stagingBuffer);
        engine.device.logical->freeMemory(stagingBufferMemory);

        texture.info.view = Device::CreateImageView(engine, texture.info.image, texture.mipLevels, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        Device::CreateSampler(engine, texture.sampler, texture.mipLevels);
        texture.imageLayout = layout;

        UpdateDescriptor(texture);
    }
}