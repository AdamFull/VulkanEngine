#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CPostProcessStage : public CRenderStage
			{
			public:
				~CPostProcessStage();
				void create() override;
				void rebuild() override;

			private:
			};
		}
	}
}