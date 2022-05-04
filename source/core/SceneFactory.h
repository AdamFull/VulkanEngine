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
        static void createComponents(std::shared_ptr<Core::Scene::CRenderObject> pRoot, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<Core::Scene::CRenderObject> createComponent(FSceneObject info);

        static std::shared_ptr<Core::Scene::CRenderObject> createCamera(FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createStaticMesh(FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createSkybox(FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createGLTFMesh(FSceneObject info);
        static std::shared_ptr<Core::Scene::CRenderObject> createLightSource(FSceneObject info);
    };
}