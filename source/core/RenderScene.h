#pragma once
#include "Resources/ResourceManager.h"
#include "graphics/scene/objects/components/SceneRootComponent.h"

namespace Engine
{
    namespace Resources { class ResourceManager; }
    
    class RenderScene
    {
    public:
        RenderScene() = default;
        ~RenderScene();
        
        void Create();
        void ReCreate();
        void Destroy();
        void AttachObject(std::shared_ptr<Core::Scene::Objects::RenderObject> object);
        inline std::shared_ptr<Core::Scene::Objects::RenderObject>& GetRoot() { return m_pRoot; }
        void CreateObjects();
        void Render(float fDeltaTime);

        std::shared_ptr<Resources::ResourceManager> GetResourceManager() { return m_pResourceManager; }
    private:
        std::shared_ptr<Core::Scene::Objects::RenderObject> m_pRoot;
        std::shared_ptr<Resources::ResourceManager> m_pResourceManager;
    };
}