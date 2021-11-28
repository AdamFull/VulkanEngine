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
                OverlaySceneGraph(std::string srName, std::shared_ptr<Scene::Objects::RenderObject> pRoot)
                {
                    srOverlayName = srName;
                    bOverlayState = true;
                    m_pRoot = pRoot;
                }
                void Draw() override;

            private:
                void CreateObject(std::string name, std::shared_ptr<Scene::Objects::RenderObject> pObject, int id);
                std::shared_ptr<Scene::Objects::RenderObject> m_pRoot;
            };
        }
    }
}