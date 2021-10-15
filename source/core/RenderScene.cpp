#include "RenderScene.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Resources/ResourceManager.h"

namespace Engine
{
    void RenderScene::Create(std::string srResourcesPath)
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
        m_pOvelray = std::make_unique<ImguiOverlay>();
        m_pResourceManager = std::make_shared<ResourceManager>();
        m_pResourceManager->Load(srResourcesPath);

        m_pOvelray->Create(UWinHandle, UDevice, USwapChain);
    }

    void RenderScene::ReCreate()
    {
        m_pOvelray->Cleanup(UDevice);
        m_pRoot->Cleanup();
        m_pOvelray->ReCreate(UDevice, USwapChain);
        m_pRoot->ReCreate();
    }

    void RenderScene::Destroy()
    {
        UDevice->GPUWait();
        if(URenderer->GetFrameStartFlag())
        {
            bool bResult;
            auto commandBuffer = URenderer->GetCurrentCommandBuffer();
            UHLInstance->EndFrame(commandBuffer, &bResult);
        }

        m_pOvelray->Destroy(UDevice);
        m_pRoot->Destroy();
    }

    void RenderScene::AttachObject(std::shared_ptr<RenderObject> object)
    {
        object->SetParent(m_pRoot);
    }

    void RenderScene::CreateObjects()
    {
        m_pRoot->Create(m_pResourceManager);
    }

    void RenderScene::Render(float fDeltaTime)
    {
        m_pRoot->Update(fDeltaTime);

        bool bResult;
        auto commandBuffer = UHLInstance->BeginFrame(&bResult);
        if(!bResult)
        {
            ReCreate();
        }

        uint32_t currentFrame = URenderer->GetImageIndex();
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();

        auto transform = m_pRoot->Find("femalebody_mesh_component")->GetTransform();
        FUniformData ubo{};
        ubo.model = transform.GetModel();
        ubo.view = camera->GetView();
        ubo.projection = camera->GetProjection();
        ubo.lightPosition = glm::vec4(camera->GetTransform().pos, 1.f);

        m_pOvelray->NewFrame();
        m_pOvelray->Update(UDevice, fDeltaTime);

        UUniform->UpdateUniformBuffer(UDevice, currentFrame, ubo);

        UHLInstance->BeginRender(commandBuffer);

        m_pRoot->Render(commandBuffer, currentFrame);

        //Imgui overlays (Demo)
        m_pOvelray->DrawFrame(UDevice, commandBuffer, currentFrame);

        UHLInstance->EndRender(commandBuffer);

        UHLInstance->EndFrame(commandBuffer, &bResult);
        if(!bResult)
        {
            ReCreate();
        }
    }
}