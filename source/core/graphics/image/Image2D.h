#pragma once
#include "Image.h"

namespace engine
{
	namespace core
	{
		/**
		 * @brief Image2D implementation
		 *
		 */
		class CImage2D : public CImage
		{
		public:
			CImage2D() = default;
			CImage2D(const fs::path& srPath, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
						vk::Filter filter = vk::Filter::eLinear);
			CImage2D(const vk::Extent2D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
						vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
						vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
						bool instantLayoutTransition = true, bool anisotropic = false, bool mipmaps = false);

		protected:
			/**
			 * @brief Creates empty image2d
			 *
			 * @param extent Texture extent
			 * @param format Vulkan format
			 * @param layout Final image layout
			 * @param usage Image usage flags
			 * @param aspect Image aspect flags
			 * @param filter Image sampler filter
			 * @param addressMode Image sampler address mode
			 * @param samples Image sample count
			 * @param instantLayoutTransition Transit image layout after creation
			 * @param anisotropic Use anisotropic filtering
			 * @param mipmaps Calculate mipmaps
			 */
			void create(const vk::Extent2D &extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
						vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, bool instantLayoutTransition, 
						bool anisotropic, bool mipmaps);
		};
	}
}