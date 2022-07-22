#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CShadowMappingStage : public CRenderStage
			{
			public:
				void create() override;
			};
		}
	}
}