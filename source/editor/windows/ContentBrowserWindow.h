#pragma once
#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		enum class EThumbnailType
		{
			eFolder,
			eText
		};

		class CContentBrowserWindow : public CWindowBase
		{
		public:
			CContentBrowserWindow();
			void draw() override;

		private:
			fs::path currentPath{};
		};
	}
}