#pragma once
#include "Objects/Components/SceneRootComponent.h"

namespace Engine
{
    class RenderScene
    {
    public:
        void Create();
        void Destroy();
        void AttachObject(std::shared_ptr<RenderObject> object);
        void CreateObjects();
        std::shared_ptr<RenderObject> FindSceneObject(std::string srName);
        void Render(float fDeltaTime);
    private:
        std::shared_ptr<SceneRootComponent> m_pRoot;
    };
}