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

using namespace Engine;
using namespace Engine::Resources;
using namespace Engine::Core::Scene;

std::unique_ptr<CRenderScene> CSceneFactory::create(std::string srScenePath)
{
    FSceneCreateInfo info = FilesystemHelper::getConfigAs<FSceneCreateInfo>(srScenePath);
    auto pRenderScene = std::make_unique<CRenderScene>();
    pRenderScene->create();

    auto pRoot = pRenderScene->getRoot();
    // TODO: check is skybox exists
    pRoot->attach(createComponent(info.skybox));

    createComponents(pRoot, info.vSceneObjects);

    return pRenderScene;
}

void CSceneFactory::createComponents(std::shared_ptr<Core::Scene::CRenderObject> pRoot, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
    {
        pRoot->attach(createComponent(object));
    }
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createComponent(FSceneObject info)
{
    std::shared_ptr<Core::Scene::CRenderObject> object;

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

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createCamera(FSceneObject info)
{
    auto camera = std::make_shared<Core::Scene::CCameraComponent>();
    camera->setTransform(info.fTransform);
    camera->setName(info.srName);
    Core::Scene::CCameraManager::inst()->attach(camera);
    return camera;
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createStaticMesh(FSceneObject info)
{
    return nullptr;
}

//Todo: do smth with code reusing
std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createSkybox(FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);

    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::CMaterialFactory::create(matInfo);
            loader->addMaterial(material);
            CResourceManager::inst()->addExisting(material->getName(), material);
        }
    }

    auto mesh = std::make_shared<Core::Scene::CMeshComponentBase>();
    loader->load(info.mesh.srSrc, info.srName);
    mesh->setTransform(info.fTransform);
    mesh->setName(info.srName);
    mesh->setMesh(loader->getMesh());
    mesh->setCullable(info.culling.bEnableCulling);
    mesh->setCullingRadius(info.culling.fSphereRadius);
    mesh->setCyllingType(info.culling.eType);

    return mesh;
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createGLTFMesh(FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);
    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::CMaterialFactory::create(matInfo);
            loader->addMaterial(material);
            CResourceManager::inst()->addExisting(material->getName(), material);
        }
    }

    auto mesh = std::make_shared<Core::Scene::CMeshComponentBase>();
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

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createLightSource(FSceneObject info)
{
    auto lightSource = CLightSourceManager::inst()->createSource(info.light.eType, info.fTransform, info.light.vColor, info.light.fAttenuation);
    return lightSource;
}