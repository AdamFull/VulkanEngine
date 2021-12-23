#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class OverlaySceneGraph : public OverlayBase
            {
            public:
                OverlaySceneGraph() = default;
                OverlaySceneGraph(std::string srName)
                {
                    srOverlayName = srName;
                    bOverlayState = false;
                }
                void Draw() override;

            private:
                void CreateObject(std::string name, int id);
            };
        }
    }
}