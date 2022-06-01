#pragma once
#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CInspectorWindow : public CWindowBase
		{
		public:
			CInspectorWindow() = default;
			void draw() override;
		};
	}
}