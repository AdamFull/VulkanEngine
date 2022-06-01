#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

#include "resources/meshes/MeshFactory.h"
#include "resources/meshes/loaders/GLTFLoader.h"
#include "resources/materials/MaterialFactory.h"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/scene/objects/components/MeshComponentBase.h"

#include "graphics/scene/lightning/LightSourceManager.h"
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
    ref_ptr<CRenderObject> object;

    switch (info.eObjectType)
    {
    case ESceneObjectType::eCamera:
        object = createCamera(info);
        break;
    case ESceneObjectType::eMeshComponent:
        object = createStaticMesh(info);
        break;
    case ESceneObjectType::eSkybox:
        object = createSkybox(info);
        break;
    case ESceneObjectType::eGltfMesh:
        object = createGLTFMesh(info);
        break;
    case ESceneObjectType::eLightSource:
        object = createLightSource(info);
        break;
    }

    if (!info.vSceneObjects.empty())
        createComponents(object, info.vSceneObjects);

    return object;
}

// Create mesh component factory!!!!!!!!!!!!!

ref_ptr<CRenderObject> CSceneFactory::createCamera(FSceneObject info)
{
    auto camera = make_ref<CCameraComponent>();
    camera->setTransform(info.fTransform);
    camera->setName(info.srName);
    CCameraManager::inst()->attach(camera);
    return camera;
}

ref_ptr<CRenderObject> CSceneFactory::createStaticMesh(FSceneObject info)
{
    return nullptr;
}

//Todo: do smth with code reusing
ref_ptr<CRenderObject> CSceneFactory::createSkybox(FSceneObject info)
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

    auto mesh = make_ref<CMeshObjectBase>();
    loader->load(info.mesh.srSrc, info.srName);
    mesh->setTransform(info.fTransform);
    mesh->setName(info.srName);
    mesh->setMesh(loader->getMesh());
    mesh->setCullable(info.culling.bEnableCulling);
    mesh->setCullingRadius(info.culling.fSphereRadius);
    mesh->setCyllingType(info.culling.eType);

    return mesh;
}

ref_ptr<CRenderObject> CSceneFactory::createGLTFMesh(FSceneObject info)
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

    auto mesh = make_ref<CMeshObjectBase>();
    mesh->setInstances(info.vInstances);
    loader->load(info.mesh.srSrc, info.srName);
    mesh->setTransform(info.fTransform);
    mesh->setName(info.srName);
    auto& loaded = loader->getMesh();
    loaded->textureRepeat(info.mesh.fRepeat);
    mesh->setMesh(loaded);
    mesh->setCullable(info.culling.bEnableCulling);
    mesh->setCullingRadius(info.culling.fSphereRadius);
    mesh->setCyllingType(info.culling.eType);

    return mesh;
}

ref_ptr<CRenderObject> CSceneFactory::createLightSource(FSceneObject info)
{
    switch (info.light.eType)
    {
    case ELightSourceType::ePoint:{
        auto lightSource = CLightSourceManager::inst()->createSource<CLightSourcePoint>();
        lightSource->setTransform(info.fTransform);
        lightSource->setColor(info.light.vColor);
        lightSource->setAttenuation(info.light.fAttenuation);
        lightSource->setIntencity(info.light.fIntencity);
        return lightSource;
        }break;
    case ELightSourceType::eDirectional:{
        auto lightSource = CLightSourceManager::inst()->createSource<CLightSourceDirectional>();
        lightSource->setTransform(info.fTransform);
        lightSource->setColor(info.light.vColor);
        lightSource->setIntencity(info.light.fIntencity);
        return lightSource;
        }break;
    case ELightSourceType::eSpot:{
        auto lightSource = CLightSourceManager::inst()->createSource<CLightSourceSpot>();
        lightSource->setTransform(info.fTransform);
        lightSource->setColor(info.light.vColor);
        lightSource->setIntencity(info.light.fIntencity);
        return lightSource;
        }break;
    }
    return nullptr;
}