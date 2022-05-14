#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
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
}