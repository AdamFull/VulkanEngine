#pragma once
#include "OverlayBase.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class COverlaySceneGraph : public COverlayBase
            {
            public:
                COverlaySceneGraph() = default;
                COverlaySceneGraph(std::string srName)
                {
                    srOverlayName = srName;
                    bOverlayState = true;
                }
                void draw() override;

            private:
                void createObject(std::string name, std::shared_ptr<Scene::CRenderObject> pObject, int id);
            };
        }
    }
}