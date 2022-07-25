#pragma once
#include "SceneConstruct.h"
#include "RenderScene.h"
#include "graphics/scene/objects/RenderObject.h"
#include "resources/meshes/loaders/GLTFLoader.h"
#include "graphics/scene/objects/components/LightSourceManager.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/scene/objects/components/ScriptingComponent.h"

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
                static void createNodes(utl::ref_ptr<core::scene::CRenderObject> &pRoot, std::vector<FSceneObject> sceneObjects);
                static utl::ref_ptr<core::scene::CRenderObject> createNode(FSceneObject info);

                static void addCamera(utl::ref_ptr<core::scene::CRenderObject>& pNode);
                static void addTexture(const std::string& srName, utl::ref_ptr<CImage>& pTexture);
                static void addMaterial(utl::ref_ptr<resources::material::CMaterialBase>& pMaterial);
                static void addLight(utl::ref_ptr<core::scene::CRenderObject>& pNode);

                template<class _Ty>
                static void createComponent(utl::ref_ptr<core::scene::CRenderObject>& pRoot, _Ty info){}
                
                // camera component
                template<>
                static void createComponent<resources::FCameraCreateInfo>(utl::ref_ptr<core::scene::CRenderObject>& pRoot, resources::FCameraCreateInfo info)
                {
                    auto camera = utl::make_ref<CCameraComponent>();
                    camera->setFieldOfView(info.fFov);
                    camera->setNearPlane(info.fNear);
                    camera->setFarPlane(info.fFar);
                    pRoot->setCamera(std::move(camera));
                    addCamera(pRoot);
                }

                // skybox component
                template<>
                static void createComponent<resources::FSkyboxCreateInfo>(utl::ref_ptr<core::scene::CRenderObject>& pRoot, resources::FSkyboxCreateInfo info)
                {
                    auto loader = utl::make_scope<resources::loaders::GLTFLoader>(false, true, info.srName);

                    auto material = resources::material::CMaterialLoader::inst()->create(info.srName);
                    auto texture = utl::make_ref<CImage>(info.environment.srSrc);
                    addTexture(info.environment.srName, texture);
                    material->addTexture(info.environment.attachment, texture);
                    addMaterial(material);
                    loader->addMaterial(material);

                    loader->load(pRoot, "meshes/cube.gltf", info.srName);
                    auto& meshNode = pRoot->find("cube");
                    auto& skybox = meshNode->getMesh();
                    skybox->setCullable(false);
                    skybox->setIsSkybox(true);
                }

                // mesh component
                template<>
                static void createComponent<resources::FMeshCreateInfo>(utl::ref_ptr<core::scene::CRenderObject>& pRoot, resources::FMeshCreateInfo info)
                {
                    auto loader = utl::make_scope<resources::loaders::GLTFLoader>(true, true, info.srName);
                    loader->load(pRoot, info.srSrc, info.srName);
                }

                // light component
                template<>
                static void createComponent<resources::FLightCreateinfo>(utl::ref_ptr<core::scene::CRenderObject>& pRoot, resources::FLightCreateinfo info)
                {
                    utl::ref_ptr<CLightComponent> lightComponent;

                    switch (info.eType)
                    {
                    case resources::ELightSourceType::eDirectional: lightComponent = utl::make_ref<CLightComponentDirectional>(); break;
                    case resources::ELightSourceType::ePoint: lightComponent = utl::make_ref<CLightComponentPoint>(); break;
                    case resources::ELightSourceType::eSpot: lightComponent = utl::make_ref<CLightComponentSpot>(); break;
                    }

                    lightComponent->setName(info.srName);
                    lightComponent->setColor(info.vColor);
                    lightComponent->setIntencity(info.fIntencity);
                    lightComponent->setRadius(info.fRadius);
                    lightComponent->setType(info.eType);
                    lightComponent->setInnerAngle(info.fInnerAngle);
                    lightComponent->setOuterAngle(info.fOuterAngle);
                    pRoot->setLight(std::move(lightComponent));
                    addLight(pRoot);
                }

                // scripting component
                template<>
                static void createComponent<resources::FStriptsCreateInfo>(utl::ref_ptr<core::scene::CRenderObject>& pRoot, resources::FStriptsCreateInfo info)
                {
                    auto sctiptComponent = utl::make_ref<CScriptingComponent>();

                    for(auto& script : info.scripts)
                        sctiptComponent->addScript(script);
                    pRoot->setScript(std::move(sctiptComponent));
                }
                
            };
        }
    }
}
