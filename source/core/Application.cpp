#include "Application.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Objects/Components/StaticMeshComponent.h"

namespace Engine
{
    void Application::Create()
    {
        m_pWindow = std::make_unique<WindowHandle>();
        m_pWindow->Create(1920, 1080, "Vulkan");

        
        InputMapper::GetInstance()->CreateAction("ServiceHandles", EActionKey::eEscape, EActionKey::eF1);
        InputMapper::GetInstance()->BindAction("ServiceHandles", EKeyState::eRelease, this, &Application::ServiceHandle);

        UHLInstance->Create(m_pWindow, "Vulkan", VK_MAKE_VERSION(1, 0, 0), "GENGINE", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        m_pCameraController = std::make_unique<CameraEditorController>();
        m_pCameraController->Create();

        m_pRenderScene = std::make_unique<RenderScene>();
        m_pRenderScene->Create();

        auto texture = std::make_shared<Texture2D>();
        texture->Create("../../assets/textures/viking_room.png");

        auto material = std::make_shared<MaterialDiffuse>();
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
        m_pRenderScene->AttachObject(mesh_component);

        auto texture1 = std::make_shared<Texture2D>();
        texture1->Create("../../assets/textures/femalebody_1.png");

        auto material1 = std::make_shared<MaterialDiffuse>();
        material1->Create(texture1);

        auto mesh1 = std::make_shared<StaticMesh>();
        mesh1->Create("../../assets/meshes/femalebody.obj");

        //Scene objects
        auto mesh_component1 = std::make_shared<StaticMeshComponent>("static_mesh_component1");
        mesh_component1->SetMesh(mesh1);
        mesh_component1->SetMaterial(material1);
        FTransform transform4;
        transform4.pos = {1.f, 0.f, 1.f};
        transform4.rot = {glm::radians(90.f), 0.f, 0.f};
        mesh_component1->SetTransform(transform4);
        m_pRenderScene->AttachObject(mesh_component1);

        //Camera
        auto camera = std::make_shared<CameraBase>("world_camera");
        FTransform transform3;
        transform3.pos = {1.f, -0.5f, 0.1f};
        transform3.rot = {0.f, glm::radians(-90.f), 0.f};
        camera->SetTransform(transform3);
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
        float delta_time{0.0f};
        while(!m_pWindow->IsShouldClose())
        {
            auto startTime = std::chrono::high_resolution_clock::now();

            m_pWindow->PollEvents();
            m_pCameraController->Update(delta_time);

            m_pRenderScene->Render(delta_time);

            //TODO: remove update from input mapper. Don't need anymore
            InputMapper::GetInstance()->Update(delta_time);

            auto currentTime = std::chrono::high_resolution_clock::now();
            delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        }
    }
}