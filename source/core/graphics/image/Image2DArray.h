#pragma once
#include "Image.h"

namespace engine
{
	namespace core
	{
		/**
		 * @brief CImage2DArray implementation
		 *
		 */
		class CImage2DArray : public CImage
		{
		public:
			CImage2DArray() = default;
			CImage2DArray(uint32_t layers, const vk::Extent2D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
						vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
						vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
						bool instantLayoutTransition = true, bool anisotropic = false, bool mipmaps = false);

		protected:
			/**
			 * @brief Creates empty CImage2DArray
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
			void create(uint32_t layers, const vk::Extent2D &extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
						vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, bool instantLayoutTransition, bool anisotropic, bool mipmaps);
		};
	}
}