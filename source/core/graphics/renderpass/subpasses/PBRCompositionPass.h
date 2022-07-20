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

			protected:
				static utl::ref_ptr<CImage> ComputeBRDFLUT(uint32_t size);
				static utl::ref_ptr<CImage> ComputeIrradiance(const utl::ref_ptr<CImage> &source, uint32_t size);
				static utl::ref_ptr<CImage> ComputePrefiltered(const utl::ref_ptr<CImage> &source, uint32_t size);

			private:
				utl::ref_ptr<CImage> m_pSkybox;

				CFuture<utl::ref_ptr<CImage>> brdf;
				CFuture<utl::ref_ptr<CImage>> irradiance;
				CFuture<utl::ref_ptr<CImage>> prefiltered;
			};
		}
	}
}