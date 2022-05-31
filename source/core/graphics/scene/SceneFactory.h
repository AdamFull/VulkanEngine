#pragma once
#include "SceneConstruct.h"
#include "RenderScene.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            struct CSceneFactory
            {
                static scope_ptr<CRenderScene> create(std::string srScenePath);
            private:
                static void createComponents(ref_ptr<Core::Scene::CRenderObject>& pRoot, std::vector<FSceneObject> sceneObjects);
                static ref_ptr<Core::Scene::CRenderObject> createComponent(FSceneObject info);

                static ref_ptr<Core::Scene::CRenderObject> createCamera(FSceneObject info);
                static ref_ptr<Core::Scene::CRenderObject> createStaticMesh(FSceneObject info);
                static ref_ptr<Core::Scene::CRenderObject> createSkybox(FSceneObject info);
                static ref_ptr<Core::Scene::CRenderObject> createGLTFMesh(FSceneObject info);
                static ref_ptr<Core::Scene::CRenderObject> createLightSource(FSceneObject info);
            };
        }
    }
}
