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

			void loadNoise(resources::ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth);

		protected:
            void generatePerlinNoise(scope_ptr<FImageCreateInfo>& texture);
            void generateFractalNoise(scope_ptr<FImageCreateInfo>& texture, uint32_t octaves, float perceptation);
		};
	}
}
