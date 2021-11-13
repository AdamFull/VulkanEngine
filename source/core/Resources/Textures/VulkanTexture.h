#pragma once
#include "Resources/ResourceBase.h"
#include "ImageLoader.h"

namespace Engine
{
    namespace Core
    {
        class Device;
        class VulkanBuffer;
    }
    
    namespace Resources
    {
        namespace Texture
        {
            struct FTextureParams
            {
                uint32_t width, height, depth;
                uint32_t mipLevels;
                uint32_t instCount;
                uint32_t layerCount;
            };

            class TextureBase : public ResourceBase
            {
            public:
                TextureBase() = default;
                TextureBase(std::shared_ptr<Core::Device> device);
                virtual ~TextureBase();

                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

                virtual void UpdateDescriptor();
                virtual void SetAttachment(ETextureAttachmentType eAttachment);
                virtual ETextureAttachmentType GetAttachment();

                virtual void CreateEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t internalFormat, bool allocate_mem = true);
                virtual void InitializeTexture(ktxTexture *info, vk::Format format, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
                void TransitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true);
                void TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
                void TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
                void BlitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);
                void CopyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<TextureBase> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout);
                virtual void WriteImageData(ktxTexture *info, vk::Format format);
                virtual void LoadFromFile(std::string srPath);
                virtual void LoadFromMemory(ktxTexture *info, vk::Format format);

                void SetImageLayout(vk::ImageLayout layout);

                virtual FTextureParams &GetParams() { return fParams; }
                virtual vk::DescriptorImageInfo &GetDescriptor() { return descriptor; }

            protected:
                virtual void GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
                static vk::ImageType TypeFromKtx(uint32_t type);

                vk::Image image;
                vk::ImageLayout imageLayout{vk::ImageLayout::eUndefined};
                vk::DeviceMemory deviceMemory;
                vk::ImageView view;
                vk::DescriptorImageInfo descriptor;
                vk::Sampler sampler;
                ETextureAttachmentType attachment;
                FTextureParams fParams;

                std::shared_ptr<Core::Device> m_device;
            };
        }
    }
}