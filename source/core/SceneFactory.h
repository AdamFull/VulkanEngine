#pragma once
#include "SceneConstruct.h"
#include "RenderScene.h"
#include "Core/Scene/Objects/RenderObject.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    struct SceneFactory
    {
        using object_creator_t = std::shared_ptr<Core::Scene::Objects::RenderObject>(FSceneObject);
        static std::unique_ptr<RenderScene> Create(std::string srScenePath);

    private:
        static void CreateComponents(std::shared_ptr<Core::Scene::Objects::RenderObject> pRoot, std::shared_ptr<Resources::ResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateComponent(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);

        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateCamera(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateStaticMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateSkybox(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateGLTFMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::Objects::RenderObject> CreateEnvironment(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info);
    };
}