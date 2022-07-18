#pragma once
#include "Image.h"
#include "noise/FractalNoise.hpp"

namespace engine
{
	namespace core
	{
		/**
		 * @brief Implementation of 3d image
		 *
		 */
		class CImage3D : public CImage
		{
		public:
			CImage3D() = default;
			CImage3D(resources::ENoisePattern ePattern, const vk::Extent3D& extent);
			virtual ~CImage3D() = default;

		protected:
			void loadNoise(resources::ENoisePattern ePattern, const vk::Extent3D& extent);
            void generatePerlinNoise(utl::scope_ptr<FImageCreateInfo>& texture);
            void generateFractalNoise(utl::scope_ptr<FImageCreateInfo>& texture, uint32_t octaves, float perceptation);
		};
	}
}
