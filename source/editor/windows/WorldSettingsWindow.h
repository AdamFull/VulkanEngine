#pragma once
#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CWorldSettingsWindow : public CWindowBase
		{
		public:
			CWorldSettingsWindow() = default;
			void draw() override;
		};
	}
}