#pragma once
#include "Resources/ResourceCunstruct.h"
#include "Core/VulkanBuffer.h"
#include "ImageLoader.h"

namespace Engine
{
    namespace Core
    {
        class Image
        {
        public:
            Image() = default;
            virtual ~Image();

            void UpdateDescriptor();
            void SetSampler(vk::Sampler& internalSampler);

            void SetImage(vk::Image& image);
            void SetView(vk::ImageView& view);

            void CreateEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t internalFormat, bool allocate_mem = true);
            void InitializeTexture(ktxTexture *info, vk::Format format, 
            vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, 
            vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);

            static vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            static vk::Format GetDepthFormat();

            static void CreateImage(vk::Image &image, vk::DeviceMemory &memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties);
            static void TransitionImageLayout(vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            static void TransitionImageLayout(vk::CommandBuffer& internalBuffer, vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            static void CopyBufferToImage(vk::Buffer &buffer, vk::Image &image, std::vector<vk::BufferImageCopy> vRegions);
            static void CopyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region);
            static vk::ImageView CreateImageView(vk::Image &pImage, vk::ImageViewCreateInfo viewInfo);
            static void CreateSampler(vk::Sampler &sampler, uint32_t mip_levels, vk::SamplerAddressMode eAddressMode, vk::Filter magFilter = vk::Filter::eLinear);

            void TransitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true);
            void TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
            void TransitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
            void BlitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);
            void CopyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<Image> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout);


            void WriteImageData(ktxTexture *info, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
            void LoadFromFile(std::string srPath);
            void LoadFromMemory(ktxTexture *info, vk::Format format);

            void SetImageLayout(vk::ImageLayout m_layout);

            const vk::Image& GetImage() const { return m_image; }
            const vk::DeviceMemory& GetMemory() const { return m_deviceMemory; }
            const vk::ImageView& GetView() const { return m_view; }
            const vk::Sampler& GetSampler() const { return m_sampler; }

            vk::DescriptorImageInfo &GetDescriptor() { return m_descriptor; }

            const vk::Extent3D& GetExtent() const { return m_extent; }
            vk::SampleCountFlagBits GetSamples() const { return m_samples; }
            vk::ImageUsageFlags GetUsage() const { return m_usage; }
            uint32_t GetMipLevels() const { return m_mipLevels; }
            uint32_t GetInstanceCount() const { return m_instCount; }
            uint32_t GetLayerCount() const { return m_layerCount; }
            vk::Format GetFormat() const { return m_format; }

            vk::Filter GetFilter() const { return m_filter; }
            vk::SamplerAddressMode GetAddressMode() const { return m_addressMode; }
            vk::ImageLayout GetImageLayout() const { return m_imageLayout; }


        protected:
            virtual void GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
            static vk::ImageType TypeFromKtx(uint32_t type);

            vk::Image m_image{VK_NULL_HANDLE};
            vk::DeviceMemory m_deviceMemory{VK_NULL_HANDLE};
            vk::ImageView m_view{VK_NULL_HANDLE};
            vk::Sampler m_sampler{VK_NULL_HANDLE};

            vk::DescriptorImageInfo m_descriptor;
            bool m_bUsingInternalSampler{false};

            vk::Extent3D m_extent;
            vk::SampleCountFlagBits m_samples;
	        vk::ImageUsageFlags m_usage;
            uint32_t m_mipLevels;
            uint32_t m_instCount;
            uint32_t m_layerCount;
            vk::Format m_format;

            vk::Filter m_filter{vk::Filter::eLinear};
	        vk::SamplerAddressMode m_addressMode;
            vk::ImageLayout m_imageLayout{vk::ImageLayout::eUndefined};
        };

        using Image2D = Image;
        using Image2DArray = Image;
        using ImageCubemap = Image;
    }
}