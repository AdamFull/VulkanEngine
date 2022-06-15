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

                static void createCamera(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createSkybox(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createGLTFMesh(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createLightSource(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
            };
        }
    }
}
