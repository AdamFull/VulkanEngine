#pragma once
#include "Resources/ResourceBase.h"
#include "filesystem/FilesystemHelper.h"

namespace Engine
{
    class Device;
    class VulkanBuffer;

    class TextureBase : public ResourceBase
    {
    public:
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        virtual void UpdateDescriptor();
        virtual void SetAttachment(ETextureAttachmentType eAttachment);
        virtual ETextureAttachmentType GetAttachment();

        virtual void LoadFromFile(std::string srPath);
        virtual void LoadFromMemory(ktxTexture* info, vk::Format format);

        virtual vk::DescriptorImageInfo& GetDescriptor() { return descriptor; }
    protected:
        virtual void GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
        static vk::ImageType TypeFromKtx(uint32_t type);

        vk::Image               image;
        vk::ImageLayout         imageLayout;
        vk::DeviceMemory        deviceMemory;
        vk::ImageView           view;
        uint32_t                width, height, channels;
        uint32_t                mipLevels;
        uint32_t                layerCount;
        vk::DescriptorImageInfo descriptor;
        vk::Sampler             sampler;
        ETextureAttachmentType  attachment;
    };
}