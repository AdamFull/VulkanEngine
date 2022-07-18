#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/buffer/VulkanBuffer.h"
#include "ImageLoaderNew.h"

namespace engine
{
	namespace core
	{
		enum class EImageType
		{
			e2D,
			e3D,
			eCubeMap,
			eArray2D,
			eArray3D,
			eArrayCube
		};

		//Image Format enum class

		/**
		 * @brief Base image implementation. Can be used as default image holder
		 *
		 */
		class CImage
		{
		public:
			CImage() = default;
			CImage(const fs::path& srPath, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
						vk::Filter filter = vk::Filter::eLinear);
			CImage(void* pData, const vk::Extent3D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
						vk::Filter filter = vk::Filter::eNearest, bool mipmap = false);
			virtual ~CImage();

			/**
			 * @brief Update image descriptor
			 *
			 */
			void updateDescriptor();
			/**
			 * @brief Set custom image sample
			 *
			 * @param internalSampler Custom image sampler
			 */
			void setSampler(vk::Sampler &internalSampler);

			/**
			 * @brief Set custom image
			 *
			 * @param image Custom image
			 */
			void setImage(vk::Image &image);
			/**
			 * @brief Set custom image view
			 *
			 * @param view Custom image view
			 */
			void setView(vk::ImageView &view);

			/**
			 * @brief Initializes texture. Creates vk::Image, vk::ImageView and vk::Sampler
			 *
			 * @param info Khronos texture object
			 * @param format Vulkan format
			 * @param flags Image usage flags
			 * @param aspect Image aspect flags
			 * @param addressMode Image sampler address mode
			 * @param filter Image sampler filter
			 * @param samples Image samples count
			 */
            void initializeTexture(utl::scope_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
								   vk::Filter filter, vk::SampleCountFlagBits samples);

			/**
			 * @brief Writing image data to GPU memory
			 *
			 * @param info Khronos texture object
			 * @param format Vulkan format
			 * @param aspect Image aspect flags
			 */
            void writeImageData(utl::scope_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
            void loadFromMemory(utl::scope_ptr<FImageCreateInfo>& info, vk::Format format,
								vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
								vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
								vk::Filter filter = vk::Filter::eLinear);

			/**
			 * @brief Finds supported format according to the passed parameters
			 *
			 * @param candidates Format candidates vector
			 * @param tiling Image tilling
			 * @param features Format features
			 * @return vk::Format
			 */
			static vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
			static std::vector<vk::Format> findSupportedFormats(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

			/**
			 * @brief Returns supported depth format
			 *
			 * @return vk::Format Depth format
			 */
			static vk::Format getDepthFormat();

			static std::vector<vk::Format> getTextureCompressionFormats();

			/**
			 * @brief Creates vk::Image object
			 *
			 * @param image Image reference
			 * @param memory Device memory reference
			 * @param createInfo Image create info
			 * @param properties Memory properties
			 */
			static void createImage(vk::Image &image, vk::DeviceMemory &memory, vk::ImageCreateInfo createInfo, vk::MemoryPropertyFlags properties);

			/**
			 * @brief Transiting image layout to another target layout
			 *
			 * @param image Image reference
			 * @param vBarriers Memory barriers
			 * @param oldLayout Old image layout
			 * @param newLayout New image layout
			 */
			static void transitionImageLayout(vk::Image &image, std::vector<vk::ImageMemoryBarrier> &vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

			/**
			 * @brief Transiting image layout to another target layout
			 *
			 * @param internalBuffer Internal command buffer
			 * @param image Image reference
			 * @param vBarriers Memory barriers
			 * @param oldLayout Old image layout
			 * @param newLayout New image layout
			 */
			static void transitionImageLayout(vk::CommandBuffer &internalBuffer, vk::Image &image, std::vector<vk::ImageMemoryBarrier> &vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

			/**
			 * @brief Transiting image layout for current image object
			 *
			 * @param newLayout New image layout
			 * @param aspectFlags Image aspect flags
			 * @param use_mips Use mipmaps or not
			 */
			void transitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true);

			/**
			 * @brief Transiting image layout for current image object
			 *
			 * @param commandBuffer Command buffer
			 * @param newLayout New image layout
			 * @param aspectFlags Image aspect flags
			 * @param use_mips Use mipmaps or not
			 * @param base_mip Base mipmap level
			 */
			void transitionImageLayout(vk::CommandBuffer &commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);

			/**
			 * @brief Transiting image layout for current image object
			 *
			 * @param commandBuffer Command buffer
			 * @param oldLayout Old image layout
			 * @param newLayout New image layout
			 * @param aspectFlags Image aspect flags
			 * @param use_mips Use mipmaps or not
			 * @param base_mip Base mipmap level
			 */
			void transitionImageLayout(vk::CommandBuffer &commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);

			/**
			 * @brief Copying buffer to image
			 *
			 * @param buffer Vulkan buffer
			 * @param image Vulkan image
			 * @param vRegions Image copy regions
			 */
			static void copyBufferToImage(vk::Buffer &buffer, vk::Image &image, std::vector<vk::BufferImageCopy> vRegions);

			/**
			 * @brief Copying source image to destination image object
			 *
			 * @param commandBuffer Command buffer
			 * @param src Source image object
			 * @param dst Destination image object
			 * @param srcLayout Source image layout
			 * @param dstLayout Destination image layout
			 * @param region Image region
			 */
			static void copyTo(vk::CommandBuffer &commandBuffer, vk::Image &src, vk::Image &dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy &region);

			/**
			 * @brief Creates image sampler
			 *
			 * @param sampler Image sampler reference
			 * @param mip_levels Mip levels
			 * @param eAddressMode Sampler address mode
			 * @param magFilter Sampler filter type
			 */
			static void createSampler(vk::Sampler &sampler, vk::Filter magFilter, vk::SamplerAddressMode eAddressMode, bool anisotropy, bool compareOp, uint32_t mipLevels);

			/**
			 * @brief Check is texture dimension is supported
			 *
			 * @param info Khronos texture object
			 * @return true if dimension is supported
			 */
            static bool isSupportedDimension(utl::scope_ptr<FImageCreateInfo>& info);

			/**
			 * @brief Blit image
			 *
			 * @param commandBuffer Command buffer
			 * @param dstLayout Destination image layout
			 * @param aspectFlags Image aspect flags
			 * @param level Image mip level
			 * @param mipWidth Mip width
			 * @param mipHeight Mip height
			 */
			void blitImage(vk::CommandBuffer &commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);

			/**
			 * @brief Copying current image to destination image
			 *
			 * @param commandBuffer Command buffer
			 * @param m_pDst Destination image object
			 * @param region Image copy region
			 * @param dstLayout Destination image layout
			 */
			void copyImageToDst(vk::CommandBuffer &commandBuffer, utl::ref_ptr<CImage> &pDst, vk::ImageCopy &region, vk::ImageLayout dstLayout);

			/**
			 * @brief Set specific image layout
			 *
			 * @param layout New image layout
			 */
			void setImageLayout(vk::ImageLayout layout);

			/**
			 * @brief Get vulkan image
			 *
			 * @return vk::Image& Vulkan image reference
			 */
			vk::Image &getImage() { return _image; }

			/**
			 * @brief Get image device memory
			 *
			 * @return const vk::DeviceMemory& Vulkan device memory
			 */
			const vk::DeviceMemory &getMemory() const { return _deviceMemory; }

			/**
			 * @brief Get image view
			 *
			 * @return const vk::ImageView& Vulkan image view
			 */
			const vk::ImageView &getView() const { return _view; }

			/**
			 * @brief Get image sampler
			 *
			 * @return const vk::Sampler& Vulkan image sampler
			 */
			const vk::Sampler &getSampler() const { return _sampler; }

			/**
			 * @brief Get image descriptor
			 *
			 * @return vk::DescriptorImageInfo& Descriptor image info
			 */
			vk::DescriptorImageInfo &getDescriptor();

			/**
			 * @brief Get image extent
			 *
			 * @return const vk::Extent3D& Image extent
			 */
			const vk::Extent3D &getExtent() const { return _extent; }

			/**
			 * @brief Get image sample count
			 *
			 * @return vk::SampleCountFlagBits Image sample count
			 */
			vk::SampleCountFlagBits getSamples() const { return _samples; }

			/**
			 * @brief Get image usage flags
			 *
			 * @return vk::ImageUsageFlags Image usage flags
			 */
			vk::ImageUsageFlags getUsage() const { return _usage; }

			/**
			 * @brief Get mip levels count
			 *
			 * @return uint32_t Mip levels
			 */
			uint32_t getMipLevels() const { return _mipLevels; }

			/**
			 * @brief Get image instance count
			 *
			 * @return uint32_t Instance ocunt
			 */
			uint32_t getInstanceCount() const { return _instCount; }

			/**
			 * @brief Get image layer count
			 *
			 * @return uint32_t Layer count
			 */
			uint32_t getLayerCount() const { return _layerCount; }

			/**
			 * @brief Get image format
			 *
			 * @return vk::Format Vulkan format
			 */
			vk::Format getFormat() const { return _format; }

			/**
			 * @brief Get image sampler filter
			 *
			 * @return vk::Filter Sampler filter
			 */
			vk::Filter getFilter() const { return _filter; }

			/**
			 * @brief Get image sampler address mode
			 *
			 * @return vk::SamplerAddressMode Sampler address mode
			 */
			vk::SamplerAddressMode getAddressMode() const { return _addressMode; }

			/**
			 * @brief Get image layout
			 *
			 * @return vk::ImageLayout Image layout
			 */
			vk::ImageLayout getImageLayout() const { return _imageLayout; }

		protected:
			/**
			 * @brief Create image from file
			 *
			 * @param srPath Source file path
			 * @param flags Image usage flags
			 * @param aspect Image aspect flags
			 * @param addressMode Image sampler address mode
			 * @param filter Image sampler filter
			 */
			void create(const fs::path& srPath, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
						vk::Filter filter = vk::Filter::eLinear);

			/**
			 * @brief Create image from data
			 *
			 * @param data Image data
			 * @param extent Image extent
			 * @param format Image format
			 * @param flags Image usage flags
			 * @param aspect Image aspect flags
			 * @param addressMode Image sampler address mode
			 * @param filter Image sampler filter
			 */
			void create(void* pData, const vk::Extent3D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
						vk::Filter filter = vk::Filter::eNearest, bool mipmap = false);

			void cleanup();

			/**
			 * @brief Generating mipmaps for image
			 *
			 * @param image Vulkan image
			 * @param mipLevels Number of mip levels
			 * @param format Vulkan format
			 * @param width Image width
			 * @param height Image height
			 * @param aspectFlags Image aspect flags
			 */
			virtual void generateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);

			/**
			 * @brief Get image type from ktx format
			 *
			 * @param type OpenGL image format
			 * @return vk::ImageType Vulkan image type
			 */
			static vk::ImageType typeFromKtx(uint32_t type);

			vk::Image _image{VK_NULL_HANDLE};
			vk::DeviceMemory _deviceMemory{VK_NULL_HANDLE};
			vk::ImageView _view{VK_NULL_HANDLE};
			vk::Sampler _sampler{VK_NULL_HANDLE};
			vk::DescriptorSet descriptorSet{VK_NULL_HANDLE};

			vk::DescriptorImageInfo _descriptor;
			bool _bUsingInternalSampler{false};
			bool bIsClean{false};

			vk::Extent3D _extent;
			vk::SampleCountFlagBits _samples;
			vk::ImageUsageFlags _usage;
			uint32_t _mipLevels;
			uint32_t _instCount;
			uint32_t _layerCount;
			vk::Format _format;
			bool enableAnisotropy{false};

			vk::Filter _filter{vk::Filter::eLinear};
			vk::SamplerAddressMode _addressMode;
			vk::ImageLayout _imageLayout{vk::ImageLayout::eUndefined};
		};
	}
}
