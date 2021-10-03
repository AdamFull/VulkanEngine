#pragma once
#include "Objects/Components/SceneRootComponent.h"
#include "Renderer/ImguiOverlay.h"

namespace Engine
{
    class WindowHandle;

    class RenderScene
    {
    public:
        void Create();
        void ReCreate();
        void Destroy();
        void AttachObject(std::shared_ptr<RenderObject> object);
        void CreateObjects();
        std::shared_ptr<RenderObject> FindSceneObject(std::string srName);
        void Render(float fDeltaTime);
    private:
        std::shared_ptr<SceneRootComponent> m_pRoot;
        std::unique_ptr<ImguiOverlay> m_pOvelray;
    };
}