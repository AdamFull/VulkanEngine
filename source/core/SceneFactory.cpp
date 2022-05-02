#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"

#include "resources/meshes/MeshFactory.h"
#include "resources/materials/MaterialFactory.h"
#include "resources/meshes/loaders/GLTFLoader.h"

#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/scene/objects/components/MeshComponentBase.h"

#include "graphics/scene/lightning/LightSourceManager.h"

using namespace Engine;
using namespace Engine::Core::Scene;

std::unique_ptr<RenderScene> SceneFactory::Create(std::string srScenePath)
{
    FSceneCreateInfo info = FilesystemHelper::getConfigAs<FSceneCreateInfo>(srScenePath);
    auto pRenderScene = std::make_unique<RenderScene>();
    pRenderScene->Create();

    auto pRoot = pRenderScene->GetRoot();
    auto pResMgr = pRenderScene->GetResourceManager();
    // TODO: check is skybox exists
    pRoot->Attach(CreateComponent(pResMgr, info.skybox));

    CreateComponents(pRoot, pResMgr, info.vSceneObjects);

    return pRenderScene;
}

void SceneFactory::CreateComponents(std::shared_ptr<Core::Scene::Objects::RenderObject> pRoot, std::shared_ptr<Resources::CResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
    {
        pRoot->Attach(CreateComponent(pResMgr, object));
    }
}

std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateComponent(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    std::shared_ptr<Core::Scene::Objects::RenderObject> object;

    switch (info.eObjectType)
    {
    case ESceneObjectType::eCamera:
        object = CreateCamera(pResMgr, info);
        break;
    case ESceneObjectType::eMeshComponent:
        object = CreateStaticMesh(pResMgr, info);
        break;
    case ESceneObjectType::eSkybox:
        object = CreateSkybox(pResMgr, info);
        break;
    case ESceneObjectType::eGltfMesh:
        object = CreateGLTFMesh(pResMgr, info);
        break;
    case ESceneObjectType::eLightSource:
        object = CreateLightSource(pResMgr, info);
        break;
    }

    if (!info.vSceneObjects.empty())
        CreateComponents(object, pResMgr, info.vSceneObjects);

    return object;
}

// Create mesh component factory!!!!!!!!!!!!!

std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateCamera(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto camera = std::make_shared<Core::Scene::Objects::Components::CameraComponent>();
    camera->SetTransform(info.fTransform);
    camera->SetName(info.srName);
    Core::Scene::Objects::Components::CameraManager::getInstance()->Attach(camera);
    return camera;
}

std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateStaticMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    /*auto mesh_component = std::make_shared<Core::Scene::Objects::Components::MeshComponentBase>();
    mesh_component->SetTransform(info.fTransform);
    mesh_component->SetName(info.srName);
    auto mesh = Resources::Mesh::MeshFactory::Create(pResMgr, info.mesh);
    mesh_component->SetMesh(mesh);
    return mesh_component;*/
    return nullptr;
}

//Todo: do smth with code reusing
std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateSkybox(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
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

    auto mesh = std::make_shared<Core::Scene::Objects::Components::MeshComponentBase>();
    loader->load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->SetTransform(info.fTransform);
    mesh->SetName(info.srName);
    mesh->SetMesh(loader->getMesh());

    return mesh;
}

std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateGLTFMesh(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
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

    auto mesh = std::make_shared<Core::Scene::Objects::Components::MeshComponentBase>();
    mesh->SetInstances(info.vInstances);
    loader->load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->SetTransform(info.fTransform);
    mesh->SetName(info.srName);
    auto& loaded = loader->getMesh();
    loaded->textureRepeat(info.mesh.fRepeat);
    mesh->SetMesh(loaded);

    return mesh;
}

std::shared_ptr<Core::Scene::Objects::RenderObject> SceneFactory::CreateLightSource(std::shared_ptr<Resources::CResourceManager> pResMgr, FSceneObject info)
{
    auto lightSource = LightSourceManager::getInstance()->CreateSource(info.light.eType, info.fTransform, info.light.vColor, info.light.fAttenuation);
    return lightSource;
}