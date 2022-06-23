#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

#include "resources/meshes/loaders/GLTFLoader.h"
#include "resources/materials/MaterialFactory.h"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/scene/objects/components/CameraManager.h"

#include "graphics/scene/objects/components/LightSourceManager.h"
#include "resources/ResourceManager.h"

using namespace engine;
using namespace engine::resources;
using namespace engine::core::scene;
using namespace resources::material;
using namespace resources::loaders;

scope_ptr<CRenderScene> CSceneFactory::create(std::string srScenePath)
{
    FSceneCreateInfo info = FilesystemHelper::getConfigAs<FSceneCreateInfo>(srScenePath);
    auto pRenderScene = make_scope<CRenderScene>();
    pRenderScene->create();

    auto pRoot = pRenderScene->getRoot();
    pRoot->setName("root");
    // TODO: check is skybox exists
    pRoot->attach(createComponent(info.skybox));

    createComponents(pRoot, info.vSceneObjects);

    return pRenderScene;
}

void CSceneFactory::createComponents(ref_ptr<CRenderObject>& pRoot, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
    {
        pRoot->attach(createComponent(object));
    }
}

ref_ptr<CRenderObject> CSceneFactory::createComponent(FSceneObject info)
{
    ref_ptr<CRenderObject> object = make_ref<CRenderObject>();
    object->setTransform(info.fTransform);
    object->setName(info.srName);

    switch (info.eObjectType)
    {
    case ESceneObjectType::eCamera:
        createCamera(object, info);
        break;
    case ESceneObjectType::eSkybox:
        createSkybox(object, info);
        break;
    case ESceneObjectType::eGltfMesh:
        createGLTFMesh(object, info);
        break;
    case ESceneObjectType::eLightSource:
        createLightSource(object, info);
        break;
    }

    if (!info.vSceneObjects.empty())
        createComponents(object, info.vSceneObjects);

    return object;
}

// Create mesh component factory!!!!!!!!!!!!!

void CSceneFactory::createCamera(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info)
{
    pRoot->setCamera(make_ref<CCameraComponent>());
    CCameraManager::inst()->attach(pRoot);
}

//Todo: do smth with code reusing
void CSceneFactory::createSkybox(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info)
{
    auto loader = make_ref<GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);

    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = CMaterialFactory::create(matInfo);
            loader->addMaterial(std::move(material));
            CResourceManager::inst()->addExisting(material->getName(), material);
        }
    }

    loader->load(pRoot, info.mesh.srSrc, info.srName);
    auto& meshNode = pRoot->find("cube");
    auto& skybox = meshNode->getMesh();
    skybox->setCullable(false);
    skybox->setIsSkybox(true);
}

void CSceneFactory::createGLTFMesh(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info)
{
    auto loader = make_ref<GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);
    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = CMaterialFactory::create(matInfo);
            loader->addMaterial(std::move(material));
            CResourceManager::inst()->addExisting(material->getName(), material);
        }
    }

    loader->load(pRoot, info.mesh.srSrc, info.srName);
}

void CSceneFactory::createLightSource(ref_ptr<core::scene::CRenderObject>& pRoot, FSceneObject info)
{
    auto lightComponent = make_ref<CLightComponent>();
    lightComponent->setName(info.srName);
    lightComponent->setColor(info.light.vColor);
    lightComponent->setIntencity(info.light.fIntencity);
    lightComponent->setType(info.light.eType);
    pRoot->setLight(std::move(lightComponent));
    CLightSourceManager::inst()->addLight(pRoot);
}