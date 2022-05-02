#pragma once
#include "OverlayBase.h"

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
                COverlaySceneGraph(std::string srName, std::shared_ptr<Scene::CRenderObject> pRoot)
                {
                    srOverlayName = srName;
                    bOverlayState = true;
                    m_pRoot = pRoot;
                }
                void draw() override;

            private:
                void createObject(std::string name, std::shared_ptr<Scene::CRenderObject> pObject, int id);
                std::shared_ptr<Scene::CRenderObject> m_pRoot;
            };
        }
    }
}