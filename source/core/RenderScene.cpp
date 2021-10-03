#include "RenderScene.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void RenderScene::Create(std::unique_ptr<WindowHandle>& winhandle)
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
        m_pOvelray = std::make_unique<ImguiOverlay>();
        m_pOvelray->Create(winhandle, UDevice, USwapChain);
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
        m_pRoot->Create();
    }

    std::shared_ptr<RenderObject> RenderScene::FindSceneObject(std::string srName)
    {
        return m_pRoot->Find(srName);
    }

    void RenderScene::Render(float fDeltaTime)
    {
        m_pRoot->Update(fDeltaTime);

        bool bResult;
        auto commandBuffer = UHLInstance->BeginFrame(&bResult);
        if(!bResult)
        {
            m_pRoot->Cleanup();
            m_pRoot->ReCreate();
        }

        uint32_t currentFrame = URenderer->GetImageIndex();
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();

        auto matrix = camera->GetProjection() * camera->GetView();
        auto transform = m_pRoot->Find("static_mesh_component1")->GetTransform();
        FUniformData ubo{};
        ubo.transform = matrix * transform.GetModel();
        ubo.worldNormal = transform.GetNormal();
        ubo.lightPosition = camera->GetTransform().pos;
        UUniform->UpdateUniformBuffer(UDevice, currentFrame, ubo);

        m_pOvelray->StartFrame();

        UHLInstance->BeginRender(commandBuffer);

        m_pRoot->Render(commandBuffer, currentFrame);

        //Imgui overlays (Demo)
        m_pOvelray->ProcessInterface();
        m_pOvelray->Render(commandBuffer);

        UHLInstance->EndRender(commandBuffer);

        UHLInstance->EndFrame(commandBuffer, &bResult);
        if(!bResult)
        {
            m_pRoot->Cleanup();
            m_pRoot->ReCreate();
        }
    }
}