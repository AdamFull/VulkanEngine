#pragma once
#include "Resources/ResourceBase.h"
#include "ImageLoader.h"

namespace Engine
{
    class Device;
    class VulkanBuffer;

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
                virtual void WriteImageData(ktxTexture *info, vk::Format format);
                virtual void LoadFromFile(std::string srPath);
                virtual void LoadFromMemory(ktxTexture *info, vk::Format format);

                virtual FTextureParams &GetParams() { return fParams; }
                virtual vk::DescriptorImageInfo &GetDescriptor() { return descriptor; }

            protected:
                virtual void GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
                static vk::ImageType TypeFromKtx(uint32_t type);

                vk::Image image;
                vk::ImageLayout imageLayout;
                vk::DeviceMemory deviceMemory;
                vk::ImageView view;
                vk::DescriptorImageInfo descriptor;
                vk::Sampler sampler;
                ETextureAttachmentType attachment;
                FTextureParams fParams;
            };
        }
    }
}