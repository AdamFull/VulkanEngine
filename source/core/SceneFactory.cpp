#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

#include "resources/meshes/MeshFactory.h"
#include "resources/meshes/loaders/GLTFLoader.h"
#include "resources/materials/MaterialFactory.h"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/scene/objects/components/MeshComponentBase.h"

#include "graphics/scene/lightning/LightSourceManager.h"

using namespace Engine;
using namespace Engine::Core::Scene;

std::unique_ptr<CRenderScene> CSceneFactory::create(std::string srScenePath)
{
    FSceneCreateInfo info = FilesystemHelper::getConfigAs<FSceneCreateInfo>(srScenePath);
    auto pRenderScene = std::make_unique<CRenderScene>();
    pRenderScene->create();

    auto pRoot = pRenderScene->getRoot();
    auto pResMgr = pRenderScene->getResourceManager();
    // TODO: check is skybox exists
    pRoot->attach(createComponent(pResMgr, info.skybox));

    createComponents(pRoot, pResMgr, info.vSceneObjects);

    return pRenderScene;
}

void CSceneFactory::createComponents(std::shared_ptr<Core::Scene::CRenderObject> pRoot, std::shared_ptr<Resources::CResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
    {
        pRoot->attach(createComponent(pResMgr, object));
    }
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createComponent(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    std::shared_ptr<Core::Scene::CRenderObject> object;

    switch (info.eObjectType)
    {
    case ESceneObjectType::eCamera:
        object = createCamera(pResMgr, info);
        break;
    case ESceneObjectType::eMeshComponent:
        object = createStaticMesh(pResMgr, info);
        break;
    case ESceneObjectType::eSkybox:
        object = createSkybox(pResMgr, info);
        break;
    case ESceneObjectType::eGltfMesh:
        object = createGLTFMesh(pResMgr, info);
        break;
    case ESceneObjectType::eLightSource:
        object = createLightSource(pResMgr, info);
        break;
    }

    if (!info.vSceneObjects.empty())
        createComponents(object, pResMgr, info.vSceneObjects);

    return object;
}

// Create mesh component factory!!!!!!!!!!!!!

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createCamera(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto camera = std::make_shared<Core::Scene::CCameraComponent>();
    camera->setTransform(info.fTransform);
    camera->setName(info.srName);
    Core::Scene::CCameraManager::getInstance()->attach(camera);
    return camera;
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createStaticMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    return nullptr;
}

//Todo: do smth with code reusing
std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createSkybox(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);

    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::CMaterialFactory::create(pResMgr, matInfo);
            loader->addMaterial(material);
            pResMgr->addExisting(material->getName(), material);
        }
    }

    auto mesh = std::make_shared<Core::Scene::CMeshComponentBase>();
    loader->load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->setTransform(info.fTransform);
    mesh->setName(info.srName);
    mesh->setMesh(loader->getMesh());

    return mesh;
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createGLTFMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);
    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::CMaterialFactory::create(pResMgr, matInfo);
            loader->addMaterial(material);
            pResMgr->addExisting(material->getName(), material);
        }
    }

    auto mesh = std::make_shared<Core::Scene::CMeshComponentBase>();
    mesh->setInstances(info.vInstances);
    loader->load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->setTransform(info.fTransform);
    mesh->setName(info.srName);
    auto& loaded = loader->getMesh();
    loaded->textureRepeat(info.mesh.fRepeat);
    mesh->setMesh(loaded);

    return mesh;
}

std::shared_ptr<Core::Scene::CRenderObject> CSceneFactory::createLightSource(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto lightSource = CLightSourceManager::getInstance()->createSource(info.light.eType, info.fTransform, info.light.vColor, info.light.fAttenuation);
    return lightSource;
}