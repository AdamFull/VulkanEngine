#pragma once
#include "Resources/ResourceManager.h"
#include "graphics/scene/objects/components/SceneRootComponent.h"

namespace Engine
{
    namespace Resources { class CResourceManager; }
    
    class CRenderScene
    {
    public:
        CRenderScene() = default;
        ~CRenderScene();
        
        void create();
        void reCreate();
        void destroy();
        void attachObject(std::shared_ptr<Core::Scene::CRenderObject> object);
        inline std::shared_ptr<Core::Scene::CRenderObject>& getRoot() { return m_pRoot; }
        void createObjects();
        void render(float fDeltaTime);
    private:
        std::shared_ptr<Core::Scene::CRenderObject> m_pRoot;
    };
}