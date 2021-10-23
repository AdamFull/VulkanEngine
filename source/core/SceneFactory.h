#pragma once
#include "SceneConstruct.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    class RenderScene;
    class RenderObject;
    class ResourceManager;

    struct SceneFactory
    {
        using object_creator_t = std::shared_ptr<RenderObject>(FSceneObject);
        static std::unique_ptr<RenderScene> Create(std::string srScenePath);
    private:
        static void CreateComponents(std::shared_ptr<RenderObject> pRoot, std::shared_ptr<ResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<RenderObject> CreateComponent(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info);

        static std::shared_ptr<RenderObject> CreateCamera(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<RenderObject> CreateStaticMesh(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<RenderObject> CreateSkybox(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<RenderObject> CreateGLTFMesh(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info);
    };
}