#pragma once
#include "Objects/Components/SceneRootComponent.h"
#include "Renderer/Overlay/ImguiOverlay.h"

namespace Engine
{
    class WindowHandle;
    class ResourceManager;

    class RenderScene
    {
    public:
        void Create(std::string srResourcesPath);
        void ReCreate();
        void Destroy();
        void AttachObject(std::shared_ptr<RenderObject> object);
        inline std::shared_ptr<RenderObject>& GetRoot() { return m_pRoot; }
        void CreateObjects();
        std::shared_ptr<RenderObject> FindSceneObject(std::string srName);
        void Render(float fDeltaTime);
    private:
        std::shared_ptr<RenderObject> m_pRoot;
        std::unique_ptr<ImguiOverlay> m_pOvelray;
        std::shared_ptr<ResourceManager> m_pResourceManager;
    };
}