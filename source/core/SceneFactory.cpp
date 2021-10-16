#include "SceneFactory.h"
#include "filesystem/FilesystemHelper.h"
#include "RenderScene.h"

#include "Objects/RenderObject.h"
#include "Objects/Components/StaticMeshComponent.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"

namespace Engine
{
    std::map<ESceneObjectType, std::function<SceneFactory::object_creator_t>> SceneFactory::m_mFactories = 
    {
        //Send to create resource manager
        {
            ESceneObjectType::eCamera,
            [](FSceneObject info)
            {
                auto camera = std::make_shared<CameraComponent>(info.srName);
                camera->SetTransform(info.fTransform);
                CameraManager::GetInstance()->Attach(camera);
                return camera;
            }
        },
        {
            ESceneObjectType::eMeshComponent,
            [](FSceneObject info)
            {
                auto mesh_component = std::make_shared<StaticMeshComponent>(info.srName);
                mesh_component->SetTransform(info.fTransform);
                mesh_component->SetResources(info.resourceBindings);
                return mesh_component;
            }
        },
        {
            ESceneObjectType::eSkybox,
            [](FSceneObject info)
            {
                auto skybox = std::make_shared<StaticMeshComponent>(info.srName);
                skybox->SetTransform(info.fTransform);
                skybox->SetResources(info.resourceBindings);
                return skybox;
            }
        }
    };

    std::unique_ptr<RenderScene> SceneFactory::Create(std::string srScenePath)
    {
        auto input = FilesystemHelper::ReadFile(srScenePath);
        auto res_json = nlohmann::json::parse(input);

        FSceneCreateInfo info;
        res_json.get_to(info);

        auto pRenderScene = std::make_unique<RenderScene>();
        pRenderScene->Create(info.resources_path);
        //TODO: check is skybox exists
        pRenderScene->SetSkybox(CreateComponent(info.skybox));

        CreateComponents(pRenderScene->GetRoot(), info.vSceneObjects);

        return pRenderScene;
    }

    void SceneFactory::CreateComponents(std::shared_ptr<RenderObject> pRoot, std::vector<FSceneObject> sceneObjects)
    {
        for(auto& object : sceneObjects)
        {
            pRoot->Attach(CreateComponent(object));
        }
    }

    std::shared_ptr<RenderObject> SceneFactory::CreateComponent(FSceneObject info)
    {
        std::shared_ptr<RenderObject> object = m_mFactories[info.eObjectType](info);
        if(!info.vSceneObjects.empty())
            CreateComponents(object, info.vSceneObjects);
        
        return object;
    }
}