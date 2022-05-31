#pragma once
#include "Resources/ResourceManager.h"
#include "graphics/scene/objects/components/SceneRootComponent.h"

namespace Engine
{
    namespace Resources { class CResourceManager; }
    namespace Core
    {
        namespace Scene
        {
            class CRenderScene
            {
            public:
                CRenderScene() = default;
                
                void create();
                void reCreate();
                void cleanup();
                void attachObject(ref_ptr<Core::Scene::CRenderObject>& object);
                inline ref_ptr<Core::Scene::CRenderObject>& getRoot() { return m_pRoot; }
                void createObjects();
                void render(float fDeltaTime);
            private:
                ref_ptr<Core::Scene::CRenderObject> m_pRoot;
            };
        }
    }
}