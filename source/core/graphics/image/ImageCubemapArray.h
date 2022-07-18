#pragma once
#include "Image.h"

namespace engine
{
	namespace core
	{
		/**
		 * @brief Image cubemap implementation
		 *
		 */
		class CImageCubemapArray : public CImage
		{
		public:
			CImageCubemapArray() = default;
			CImageCubemapArray(uint32_t layers, const vk::Extent2D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
						vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
						vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
						bool instantLayoutTransition = true, bool anisotropic = false, bool mipmaps = false);
			virtual ~CImageCubemapArray() = default;

		protected:
			/**
			 * @brief Creates empty cubemap texture
			 *
			 * @param layers Number of array layers
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
			void create(uint32_t layers, const vk::Extent2D &extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
						vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
						vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
						bool instantLayoutTransition = true, bool anisotropic = false, bool mipmaps = false);
		};
	}
}