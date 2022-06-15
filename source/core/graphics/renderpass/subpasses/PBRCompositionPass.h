#pragma once
#include "util/threading.hpp"
#include "graphics/renderpass/Subpass.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/components/LightComponent.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CPBRCompositionPass : public CSubpass
			{
			public:
				void create() override;
				void reCreate() override;
				void render(vk::CommandBuffer &commandBuffer) override;
				void cleanup() override;

			protected:
				static ref_ptr<CImage> ComputeBRDFLUT(uint32_t size);
				static ref_ptr<CImage> ComputeIrradiance(const ref_ptr<CImage> &source, uint32_t size);
				static ref_ptr<CImage> ComputePrefiltered(const ref_ptr<CImage> &source, uint32_t size);

			private:
				ref_ptr<CImage> m_pSkybox;

				CFuture<ref_ptr<CImage>> brdf;
				CFuture<ref_ptr<CImage>> irradiance;
				CFuture<ref_ptr<CImage>> prefiltered;
			};
		}
	}
}