#pragma once
#include "WindowBase.h"
#include "graphics/descriptor/DescriptorSet.h"

namespace engine
{
	namespace editor
	{
		class CViewportWindow : public CWindowBase
		{
		public:
			CViewportWindow() = default;

			void create() override;
			void reCreate() override;
			void draw() override;

		private:
			void drawViewport(float offsetx, float offsety);
			void drawManipulator(float offsetx, float offsety, float sizex, float sizey);
			void drawOverlay(float offsetx, float offsety);
			utl::scope_ptr<core::descriptor::CDescriptorSet> pDescriptorSet;
			std::vector<VkDescriptorSet> descriptors;
			float overlayX{300.0}, overlayY{50.f};
		};
	}
}