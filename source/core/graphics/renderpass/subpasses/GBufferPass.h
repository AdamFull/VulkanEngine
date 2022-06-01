#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CGBufferPass : public CSubpass
			{
			public:
				void create() override;
				void reCreate() override;
				void render(vk::CommandBuffer &commandBuffer) override;
				void cleanup() override;
			};
		}
	}
}