#include "Application.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Materials/MaterialDiffuse.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Objects/Components/StaticMeshComponent.h"

namespace Engine
{
    void Application::Create()
    {
        InputMapper::GetInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::GetInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        FEngineCreateInfo createInfo;
        createInfo.window.width = 1920;
        createInfo.window.height = 1080;
        createInfo.window.name = "Vulkan";
        createInfo.appName = "Vulkan";
        createInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
        createInfo.engineName = "IncenerateEngine";
        createInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        createInfo.apiVersion = VK_API_VERSION_1_0;
        UHLInstance->Create(createInfo);

        m_pCameraController = std::make_unique<CameraEditorController>();
        m_pCameraController->Create();

        m_pRenderScene = std::make_unique<RenderScene>();
        m_pRenderScene->Create();

        /*auto material = std::make_shared<MaterialDiffuse>();
        material->Create(texture);

        auto mesh = std::make_shared<StaticMesh>();
        mesh->Create("../../assets/meshes/viking_room.obj");

        //Scene objects
        auto mesh_component = std::make_shared<StaticMeshComponent>("static_mesh_component");
        mesh_component->SetMesh(mesh);
        mesh_component->SetMaterial(material);
        FTransform transform1;
        transform1.rot = {glm::radians(90.f), 0.f, 0.f};
        mesh_component->SetTransform(transform1);
        m_pRenderScene->AttachObject(mesh_component);*/

        auto texture_diffuse = std::make_shared<Texture2D>();
        texture_diffuse->Create("../../assets/textures/femalebody_1.png");

        auto texture_normal = std::make_shared<Texture2D>();
        texture_normal->Create("../../assets/textures/femalebody_1_msn.png");

        auto texture_specular = std::make_shared<Texture2D>();
        texture_specular->Create("../../assets/textures/femalebody_1_s.png");

        auto material_diffuse = std::make_shared<MaterialDiffuse>();
        material_diffuse->Create(texture_diffuse, texture_normal, texture_specular);

        auto body_mesh = std::make_shared<StaticMesh>();
        body_mesh->Create("../../assets/meshes/femalebody.obj");

        //Scene objects
        auto mesh_component = std::make_shared<StaticMeshComponent>("static_mesh_component1");
        mesh_component->SetMesh(body_mesh);
        mesh_component->SetMaterial(material_diffuse);
        mesh_component->SetPosition({0.f, 0.f, 0.f});
        mesh_component->SetRotation({0.f, 0.f, glm::radians(180.f)});
        m_pRenderScene->AttachObject(mesh_component);

        //Camera
        auto camera = std::make_shared<CameraBase>("world_camera");
        camera->SetPosition({0.f, -2.4f, -1.8f});
        m_pRenderScene->AttachObject(camera);
        CameraManager::GetInstance()->Attach(camera);
    }

    void Application::ServiceHandle(EActionKey eKey)
    {
        switch (eKey)
        {
        case EActionKey::eEscape: 
        {
            m_pRenderScene->Destroy();
            std::exit(10); 
        }break;
        default: break;
        }
    }

    void Application::run()
    {
        m_pRenderScene->CreateObjects();
        float delta_time{0.001f};
        while(!UWinHandle->IsShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            UWinHandle->PollEvents();
            m_pCameraController->Update(delta_time);

            m_pRenderScene->Render(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            InputMapper::GetInstance()->Update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}