#pragma once
#include "graphics/renderpass/RenderStage.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CPresentFinalStage : public CRenderStage
			{
			public:
				void create() override;

			private:
			};
		}
	}
}