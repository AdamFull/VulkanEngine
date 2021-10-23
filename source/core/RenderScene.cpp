#include "RenderScene.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanVBO.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Resources/ResourceManager.h"

namespace Engine
{
    void RenderScene::Create()
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
        m_pOvelray = std::make_unique<ImguiOverlay>();
        m_pResourceManager = std::make_shared<ResourceManager>();

        m_pOvelray->Create(UWinHandle, UDevice, USwapChain);

        //TODO: move to another place
        auto pEmptyTexture = std::make_shared<TextureBase>();
        pEmptyTexture->CreateEmptyTexture(512, 512, 1, 2);
        m_pResourceManager->AddExisting("no_texture", pEmptyTexture);
    }

    void RenderScene::ReCreate()
    {
        m_pOvelray->Cleanup(UDevice);
        m_pRoot->Cleanup();
        m_pOvelray->ReCreate(UDevice, USwapChain);

        if(m_pSkybox)
            m_pSkybox->ReCreate();

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
        
        if(m_pSkybox)
            m_pSkybox->Destroy();

        m_pRoot->Destroy();
    }

    void RenderScene::AttachObject(std::shared_ptr<RenderObject> object)
    {
        object->SetParent(m_pRoot);
    }

    void RenderScene::SetSkybox(std::shared_ptr<RenderObject> pSkybox)
    {
        m_pSkybox = pSkybox;
    }

    void RenderScene::CreateObjects()
    {
        if(m_pSkybox)
            m_pSkybox->Create(m_pResourceManager);

        m_pRoot->Create(m_pResourceManager);
        UVBO->Create(UDevice);
    }

    void RenderScene::Render(float fDeltaTime)
    {
        if(m_pSkybox)
            m_pSkybox->Update(fDeltaTime);

        m_pRoot->Update(fDeltaTime);

        bool bResult;
        auto commandBuffer = UHLInstance->BeginFrame(&bResult);
        if(!bResult)
        {
            ReCreate();
        }

        uint32_t currentFrame = URenderer->GetImageIndex();

        m_pOvelray->NewFrame();
        m_pOvelray->Update(UDevice, fDeltaTime);

        UHLInstance->BeginRender(commandBuffer);

        UVBO->Bind(commandBuffer);

        if(m_pSkybox)
            m_pSkybox->Render(commandBuffer, currentFrame);

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