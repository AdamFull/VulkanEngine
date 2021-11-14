#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"
#include "RenderScene.h"

#include "Resources/ResourceManager.h"

#include "Resources/Meshes/MeshFactory.h"
#include "Resources/Materials/MaterialFactory.h"
#include "Resources/Meshes/Loaders/GLTFLoader.h"
#include "Resources/Textures/TextureFactory.h"

#include "Objects/RenderObject.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Objects/Components/MeshComponentBase.h"
#include "Objects/Components/MeshVolumeComponent.h"

using namespace Engine;

std::unique_ptr<RenderScene> SceneFactory::Create(std::string srScenePath)
{
    auto input = FilesystemHelper::ReadFile(srScenePath);
    auto res_json = nlohmann::json::parse(input);

    FSceneCreateInfo info;
    res_json.get_to(info);

    auto pRenderScene = std::make_unique<RenderScene>();
    pRenderScene->Create();

    auto pRoot = pRenderScene->GetRoot();
    auto pResMgr = pRenderScene->GetResourceManager();
    // TODO: check is skybox exists
    pRenderScene->SetSkybox(CreateComponent(pResMgr, info.skybox));
    //TODO: Made it better
    pRenderScene->SetEnvironment(CreateComponent(pResMgr, info.environment));

    CreateComponents(pRoot, pResMgr, info.vSceneObjects);

    return pRenderScene;
}

void SceneFactory::CreateComponents(std::shared_ptr<Objects::RenderObject> pRoot, std::shared_ptr<Resources::ResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
    {
        pRoot->Attach(CreateComponent(pResMgr, object));
    }
}

std::shared_ptr<Objects::RenderObject> SceneFactory::CreateComponent(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    std::shared_ptr<Objects::RenderObject> object;

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
    case ESceneObjectType::eEnvironment:
        object = CreateEnvironment(pResMgr, info);
        break;
    }

    if (!info.vSceneObjects.empty())
        CreateComponents(object, pResMgr, info.vSceneObjects);

    return object;
}

// Create mesh component factory!!!!!!!!!!!!!

std::shared_ptr<Objects::RenderObject> SceneFactory::CreateCamera(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    auto camera = std::make_shared<Objects::Components::CameraComponent>();
    camera->SetTransform(info.fTransform);
    camera->SetName(info.srName);
    Objects::Components::CameraManager::GetInstance()->Attach(camera);
    return camera;
}

std::shared_ptr<Objects::RenderObject> SceneFactory::CreateStaticMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    /*auto mesh_component = std::make_shared<Objects::Components::MeshComponentBase>();
    mesh_component->SetTransform(info.fTransform);
    mesh_component->SetName(info.srName);
    auto mesh = Resources::Mesh::MeshFactory::Create(pResMgr, info.mesh);
    mesh_component->SetMesh(mesh);
    return mesh_component;*/
    return nullptr;
}

//Todo: do smth with code reusing
std::shared_ptr<Objects::RenderObject> SceneFactory::CreateSkybox(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);

    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::MaterialFactory::Create(pResMgr, matInfo);
            loader->AddMaterial(material);
            pResMgr->AddExisting(material->GetName(), material);
        }
    }

    auto mesh = std::make_shared<Objects::Components::MeshComponentBase>();
    loader->Load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->SetTransform(info.fTransform);
    mesh->SetName(info.srName);
    mesh->SetMesh(loader->GetMesh());

    return mesh;
}

std::shared_ptr<Objects::RenderObject> SceneFactory::CreateGLTFMesh(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(info.mesh.bUseIncludedMaterial, true, info.srName, info.srUseVolume);
    if (!info.mesh.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.mesh.vMaterials)
        {
            auto material = Resources::Material::MaterialFactory::Create(pResMgr, matInfo);
            loader->AddMaterial(material);
            pResMgr->AddExisting(material->GetName(), material);
        }
    }

    auto mesh = std::make_shared<Objects::Components::MeshComponentBase>();
    loader->Load(info.mesh.srSrc, info.srName, pResMgr);
    mesh->SetTransform(info.fTransform);
    mesh->SetName(info.srName);
    mesh->SetMesh(loader->GetMesh());

    return mesh;
}

std::shared_ptr<Objects::RenderObject> SceneFactory::CreateEnvironment(std::shared_ptr<Resources::ResourceManager> pResMgr, FSceneObject info)
{
    auto loader = std::make_shared<Resources::Loaders::GLTFLoader>(true, false, info.srName, info.srUseVolume);

    auto environment = std::make_shared<Objects::Components::MeshVolumeComponent>();
    loader->Load(info.mesh.srSrc, info.srName, pResMgr);
    environment->SetName(info.srName);
    environment->SetMesh(loader->GetMesh());
    environment->SetTexture(Resources::Texture::TextureFactory::Create(pResMgr, info.texture));

    return environment;
}