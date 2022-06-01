#pragma once
#include "graphics/renderpass/Subpass.h"

namespace engine
{
	namespace core
	{
		namespace render
		{
			class CBlurPass : public CSubpass
			{
			public:
				CBlurPass() = default;
				explicit CBlurPass(int direction, const std::string &ref_name) : direction(direction), imageReferenceName(ref_name) {}
				void create() override;
				void reCreate() override;
				void render(vk::CommandBuffer &commandBuffer) override;
				void cleanup() override;

			private:
				int direction{-1};
				std::string imageReferenceName{""};
			};
		}
	}
}