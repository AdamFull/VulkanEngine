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
                static utl::scope_ptr<CRenderScene> create(std::string srScenePath);

            private:
                static void createComponents(utl::ref_ptr<core::scene::CRenderObject> &pRoot, std::vector<FSceneObject> sceneObjects);
                static utl::ref_ptr<core::scene::CRenderObject> createComponent(FSceneObject info);

                static void createCamera(utl::ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createSkybox(utl::ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createGLTFMesh(utl::ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
                static void createLightSource(utl::ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info);
            };
        }
    }
}
