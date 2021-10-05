#pragma once
#include "OverlayBase.h"

namespace Engine
{
    class OverlaySceneGraph : public OverlayBase
    {
    public:
        OverlaySceneGraph() = default;
        OverlaySceneGraph(std::string srName)
        {
            srOverlayName = srName;
        }
        void Draw() override;
    private:
        void CreateObject(std::string name, int id);
    };
}