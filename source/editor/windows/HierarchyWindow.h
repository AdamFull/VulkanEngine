#pragma once
#include "WindowBase.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
	namespace editor
	{
		class CHierarchyWindow : public CWindowBase
		{
		public:
			CHierarchyWindow() = default;
			void draw() override;

		private:
			void buildHierarchy(ref_ptr<core::scene::CRenderObject> &pObject);
		};
	}
}