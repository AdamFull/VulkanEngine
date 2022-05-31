#pragma once
#include "WindowBase.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Editor
        {
            class CHierarchyWindow : public CWindowBase
            {
            public:
                CHierarchyWindow() = default;
                void draw() override;

            private:
                void buildHierarchy(ref_ptr<Scene::CRenderObject>& pObject);
            };
        }
    }
}