#pragma once
#include "Objects/Components/SceneRootComponent.h"

namespace Engine
{
    namespace Core
    {
        namespace Window  { class WindowHandle; }
    }
    
    namespace Resources { class ResourceManager; }
    
    class RenderScene
    {
    public:
        RenderScene() = default;
        ~RenderScene();
        
        void Create();
        void ReCreate();
        void Destroy();
        void AttachObject(std::shared_ptr<Objects::RenderObject> object);
        void SetSkybox(std::shared_ptr<Objects::RenderObject> pSkybox);
        void SetEnvironment(std::shared_ptr<Objects::RenderObject> pEnvironment);
        inline std::shared_ptr<Objects::RenderObject>& GetRoot() { return m_pRoot; }
        void CreateObjects();
        void Render(float fDeltaTime);

        std::shared_ptr<Resources::ResourceManager> GetResourceManager() { return m_pResourceManager; }
    private:
        std::shared_ptr<Objects::RenderObject> m_pSkybox;
        std::shared_ptr<Objects::RenderObject> m_pEnvironment;
        std::shared_ptr<Objects::RenderObject> m_pRoot;
        std::shared_ptr<Resources::ResourceManager> m_pResourceManager;
    };
}