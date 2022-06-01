#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CDownsamplePass : public CSubpass
			{
			public:
				CDownsamplePass() = default;
				CDownsamplePass(const std::string &textureName) : targetTextureName(textureName) {}
				void create() override;
				void reCreate() override;
				void render(vk::CommandBuffer &commandBuffer) override;
				void cleanup() override;

			private:
				std::string targetTextureName{};
			};
		}
	}
}