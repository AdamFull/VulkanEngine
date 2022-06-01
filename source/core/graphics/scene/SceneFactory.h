#pragma once
#include "SceneConstruct.h"
#include "RenderScene.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            struct CSceneFactory
            {
                static scope_ptr<CRenderScene> create(std::string srScenePath);

            private:
                static void createComponents(ref_ptr<core::scene::CRenderObject> &pRoot, std::vector<FSceneObject> sceneObjects);
                static ref_ptr<core::scene::CRenderObject> createComponent(FSceneObject info);

                static ref_ptr<core::scene::CRenderObject> createCamera(FSceneObject info);
                static ref_ptr<core::scene::CRenderObject> createStaticMesh(FSceneObject info);
                static ref_ptr<core::scene::CRenderObject> createSkybox(FSceneObject info);
                static ref_ptr<core::scene::CRenderObject> createGLTFMesh(FSceneObject info);
                static ref_ptr<core::scene::CRenderObject> createLightSource(FSceneObject info);
            };
        }
    }
}
