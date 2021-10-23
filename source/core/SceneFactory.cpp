#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"
#include "RenderScene.h"

#include "Resources/ResourceManager.h"

#include "Resources/Meshes/MeshFactory.h"
#include "Resources/Materials/MaterialFactory.h"
#include "Resources/Meshes/Loaders/GLTFLoader.h"

#include "Objects/RenderObject.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Objects/Components/MeshComponentBase.h"

namespace Engine
{
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
        //TODO: check is skybox exists
        pRenderScene->SetSkybox(CreateComponent(pResMgr, info.skybox));

        CreateComponents(pRoot, pResMgr, info.vSceneObjects);

        return pRenderScene;
    }

    void SceneFactory::CreateComponents(std::shared_ptr<RenderObject> pRoot, std::shared_ptr<ResourceManager> pResMgr, std::vector<FSceneObject> sceneObjects)
    {
        for(auto& object : sceneObjects)
        {
            pRoot->Attach(CreateComponent(pResMgr, object));
        }
    }

    std::shared_ptr<RenderObject> SceneFactory::CreateComponent(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info)
    {
        std::shared_ptr<RenderObject> object;

        switch (info.eObjectType)
        {
        case ESceneObjectType::eCamera: object = CreateCamera(pResMgr, info); break;
        case ESceneObjectType::eMeshComponent: object = CreateStaticMesh(pResMgr, info); break;
        case ESceneObjectType::eSkybox: object = CreateSkybox(pResMgr, info); break;
        case ESceneObjectType::eGltfMesh: object = CreateGLTFMesh(pResMgr, info); break;
        }

        if(!info.vSceneObjects.empty())
            CreateComponents(object, pResMgr, info.vSceneObjects);
        
        return object;
    }

    //Create mesh component factory!!!!!!!!!!!!!

    std::shared_ptr<RenderObject> SceneFactory::CreateCamera(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info)
    {
        auto camera = std::make_shared<CameraComponent>();
        camera->SetTransform(info.fTransform);
        camera->SetName(info.srName);
        CameraManager::GetInstance()->Attach(camera);
        return camera;
    }

    std::shared_ptr<RenderObject> SceneFactory::CreateStaticMesh(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info)
    {
        auto mesh_component = std::make_shared<MeshComponentBase>();
        mesh_component->SetTransform(info.fTransform);
        mesh_component->SetName(info.srName);
        auto mesh = MeshFactory::Create(pResMgr, info.mesh);
        mesh_component->SetMesh(mesh);
        return mesh_component;
    }

    std::shared_ptr<RenderObject> SceneFactory::CreateSkybox(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info)
    {
        auto skybox = std::make_shared<MeshComponentBase>();
        skybox->SetTransform(info.fTransform);
        skybox->SetName(info.srName);
        auto mesh = MeshFactory::Create(pResMgr, info.mesh);
        skybox->SetMesh(mesh);
        return skybox;
    }

    std::shared_ptr<RenderObject> SceneFactory::CreateGLTFMesh(std::shared_ptr<ResourceManager> pResMgr, FSceneObject info)
    {
        auto tmp = std::make_shared<Loaders::Model::GLTFLoader::LoaderTemporaryObject>();
        tmp->bLoadMaterials = info.mesh.bUseIncludedMaterial;
        if(!info.mesh.bUseIncludedMaterial)
        {
            for(auto& matInfo : info.mesh.vMaterials)
            {
                auto material = MaterialFactory::Create(pResMgr, matInfo);
                tmp->vMaterials.emplace_back(material);
                pResMgr->AddExisting(material->GetName(), material);
            }
        }


        auto mesh_node = Loaders::Model::GLTFLoader::Load(info.mesh.srSrc, info.srName, tmp, pResMgr);
        mesh_node->SetTransform(info.fTransform);
        mesh_node->SetName(info.srName);

        return mesh_node;
    }
}