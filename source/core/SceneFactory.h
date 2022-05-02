#pragma once
#include "SceneConstruct.h"
#include "RenderScene.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    struct CSceneFactory
    {
        using object_creator_t = std::shared_ptr<Core::Scene::CRenderObject>(FSceneObject);
        static std::unique_ptr<CRenderScene> create(std::string srScenePath);

    private:
        static void createComponents(std::shared_ptr<Core::Scene::CRenderObject> pRoot, std::shared_ptr<Resources::CResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<Core::Scene::CRenderObject> createComponent(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);

        static std::shared_ptr<Core::Scene::CRenderObject> createCamera(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createStaticMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createSkybox(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createGLTFMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createLightSource(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info);
    };
}