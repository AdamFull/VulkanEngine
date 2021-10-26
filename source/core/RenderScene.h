#pragma once
#include "Objects/Components/SceneRootComponent.h"

namespace Engine
{
    class WindowHandle;
    class ResourceManager;

    class RenderScene
    {
    public:
        void Create();
        void ReCreate();
        void Destroy();
        void AttachObject(std::shared_ptr<RenderObject> object);
        void SetSkybox(std::shared_ptr<RenderObject> pSkybox);
        inline std::shared_ptr<RenderObject>& GetRoot() { return m_pRoot; }
        void CreateObjects();
        void Render(float fDeltaTime);

        std::shared_ptr<ResourceManager> GetResourceManager() { return m_pResourceManager; }
    private:
        std::shared_ptr<RenderObject> m_pSkybox;
        std::shared_ptr<RenderObject> m_pRoot;
        std::shared_ptr<ResourceManager> m_pResourceManager;
    };
}