#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CImguiPass : public CSubpass
			{
			public:
				void create() override;
				void reCreate() override;
				void render(vk::CommandBuffer &commandBuffer) override;
			};
		}
	}
}