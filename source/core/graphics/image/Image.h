#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/buffer/VulkanBuffer.h"
#include "ImageLoader.h"

namespace Engine
{
    namespace Core
    {
        class CImage
        {
        public:
            CImage() = default;
            virtual ~CImage();

            void updateDescriptor();
            void setSampler(vk::Sampler& internalSampler);

            void setImage(vk::Image& image);
            void setView(vk::ImageView& view);

            void createEmptyTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t dims, uint32_t internalFormat, bool allocate_mem = true);
            void initializeTexture(ktxTexture *info, vk::Format format, 
            vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, 
            vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);

            static vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            static vk::Format getDepthFormat();

            static void createImage(vk::Image &image, vk::DeviceMemory &memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties);
            static void transitionImageLayout(vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            static void transitionImageLayout(vk::CommandBuffer& internalBuffer, vk::Image &image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            static void copyBufferToImage(vk::Buffer &buffer, vk::Image &image, std::vector<vk::BufferImageCopy> vRegions);
            static void copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region);
            static vk::ImageView createImageView(vk::Image &pImage, vk::ImageViewCreateInfo viewInfo);
            static void createSampler(vk::Sampler &sampler, uint32_t mip_levels, vk::SamplerAddressMode eAddressMode, vk::Filter magFilter = vk::Filter::eLinear);

            static bool isSupportedDimension(ktxTexture *info);

            void transitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true);
            void transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
            void transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
            void blitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);
            void copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage> m_pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout);


            void writeImageData(ktxTexture *info, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
            void loadFromFile(std::string srPath);
            void loadFromMemory(ktxTexture *info, vk::Format format);

            void setImageLayout(vk::ImageLayout layout);

            vk::Image& getImage() { return _image; }
            const vk::DeviceMemory& getMemory() const { return _deviceMemory; }
            const vk::ImageView& getView() const { return _view; }
            const vk::Sampler& getSampler() const { return _sampler; }

            vk::DescriptorImageInfo &getDescriptor() { return _descriptor; }

            const vk::Extent3D& getExtent() const { return _extent; }
            vk::SampleCountFlagBits getSamples() const { return _samples; }
            vk::ImageUsageFlags getUsage() const { return _usage; }
            uint32_t getMipLevels() const { return _mipLevels; }
            uint32_t getInstanceCount() const { return _instCount; }
            uint32_t getLayerCount() const { return _layerCount; }
            vk::Format getFormat() const { return _format; }

            vk::Filter getFilter() const { return _filter; }
            vk::SamplerAddressMode getAddressMode() const { return _addressMode; }
            vk::ImageLayout getImageLayout() const { return _imageLayout; }


        protected:
            virtual void generateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);
            static vk::ImageType typeFromKtx(uint32_t type);

            vk::Image _image{VK_NULL_HANDLE};
            vk::DeviceMemory _deviceMemory{VK_NULL_HANDLE};
            vk::ImageView _view{VK_NULL_HANDLE};
            vk::Sampler _sampler{VK_NULL_HANDLE};

            vk::DescriptorImageInfo _descriptor;
            bool _bUsingInternalSampler{false};

            vk::Extent3D _extent;
            vk::SampleCountFlagBits _samples;
	        vk::ImageUsageFlags _usage;
            uint32_t _mipLevels;
            uint32_t _instCount;
            uint32_t _layerCount;
            vk::Format _format;

            vk::Filter _filter{vk::Filter::eLinear};
	        vk::SamplerAddressMode _addressMode;
            vk::ImageLayout _imageLayout{vk::ImageLayout::eUndefined};
        };

        using CImage2D = CImage;
        using CImage2DArray = CImage;
        using CImageCubemap = CImage;
    }
}