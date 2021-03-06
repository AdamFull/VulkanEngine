#pragma once
#include <imgui.h>

namespace engine
{
	namespace editor
	{
		class CWindowBase
		{
		public:
			virtual ~CWindowBase() {}
			CWindowBase() = default;

			virtual void create() {}
			virtual void reCreate() {}
			virtual void setState(bool bState) { bOverlayState = bState; }
			virtual void draw();

			const bool isEnabled() const { return bOverlayState; }
			void toggleEnable() { bOverlayState = !bOverlayState; }

		protected:
			bool bOverlayState{true};
		};
	}
}