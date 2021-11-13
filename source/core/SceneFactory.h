#pragma once
#include "SceneConstruct.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    class RenderScene;
    namespace Resources { class ResourceManager; }
    namespace Objects { class RenderObject; }

    struct SceneFactory
    {
        using object_creator_t = std::shared_ptr<Objects::RenderObject>(FSceneObject);
        static std::unique_ptr<RenderScene> Create(std::string srScenePath);
    private:
        static void CreateComponents(std::shared_ptr<Objects::RenderObject> pRoot, std::shared_ptr<Resources::ResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<Objects::RenderObject> CreateComponent(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);

        static std::shared_ptr<Objects::RenderObject> CreateCamera(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Objects::RenderObject> CreateStaticMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Objects::RenderObject> CreateSkybox(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Objects::RenderObject> CreateGLTFMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Objects::RenderObject> CreateEnvironment(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
    };
}