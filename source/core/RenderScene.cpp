#include "RenderScene.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"

namespace Engine
{
    void RenderScene::Create()
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
        m_pOvelray = std::make_unique<ImguiOverlay>();
        m_pOvelray->Create(UDevice, USwapChain);

        InputMapper::GetInstance()->CreateAction("OverlayMouse", EActionKey::eMouseLeft, EActionKey::eMouseRight, EActionKey::eLeftShift, EActionKey::eLeftControl);
        InputMapper::GetInstance()->CreateAction("OverlayMousePosition", EActionKey::eCursorOriginal);
        InputMapper::GetInstance()->BindAction("OverlayMouse", EKeyState::ePressed, m_pOvelray.get(), &ImguiOverlay::ProcessKeys);
        InputMapper::GetInstance()->BindAction("OverlayMouse", EKeyState::eRelease, m_pOvelray.get(), &ImguiOverlay::ProcessKeys);
        InputMapper::GetInstance()->BindAxis("OverlayMousePosition", m_pOvelray.get(), &ImguiOverlay::ProcessCursor);
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
            ReCreate();
        }

        uint32_t currentFrame = URenderer->GetImageIndex();
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();

        auto transform = m_pRoot->Find("static_mesh_component1")->GetTransform();
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