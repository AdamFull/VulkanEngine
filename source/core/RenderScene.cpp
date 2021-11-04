#include "RenderScene.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Core/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Resources/ResourceManager.h"
#include "Core/Overlay/ImguiOverlay.h"

using namespace Engine;

void RenderScene::Create()
{
    m_pRoot = std::make_shared<Objects::Components::SceneRootComponent>();
    m_pResourceManager = std::make_shared<Resources::ResourceManager>();

    // TODO: move to another place
    auto pEmptyTexture = std::make_shared<Resources::Texture::TextureBase>();
    pEmptyTexture->CreateEmptyTexture(512, 512, 1, 2);
    m_pResourceManager->AddExisting("no_texture", pEmptyTexture);
}

void RenderScene::ReCreate()
{
    m_pRoot->Cleanup();

    if (m_pSkybox)
        m_pSkybox->ReCreate();

    m_pRoot->ReCreate();
}

void RenderScene::Destroy()
{
    UDevice->GPUWait();
    if (URenderer->GetFrameStartFlag())
    {
        bool bResult;
        auto commandBuffer = URenderer->GetCurrentCommandBuffer();
        UHLInstance->EndFrame(commandBuffer, &bResult);
    }

    if (m_pSkybox)
        m_pSkybox->Destroy();

    m_pRoot->Destroy();
}

void RenderScene::AttachObject(std::shared_ptr<Objects::RenderObject> object)
{
    object->SetParent(m_pRoot);
}

void RenderScene::SetSkybox(std::shared_ptr<Objects::RenderObject> pSkybox)
{
    m_pSkybox = pSkybox;
}

void RenderScene::CreateObjects()
{
    if (m_pSkybox)
        m_pSkybox->Create(m_pResourceManager);

    m_pRoot->Create(m_pResourceManager);
    UVBO->Create(UDevice);
}

void RenderScene::Render(float fDeltaTime)
{
    if (m_pSkybox)
        m_pSkybox->Update(fDeltaTime);

    m_pRoot->Update(fDeltaTime);

    bool bResult;
    auto commandBuffer = UHLInstance->BeginFrame(&bResult);
    if (!bResult)
    {
        ReCreate();
    }

    uint32_t currentFrame = URenderer->GetImageIndex();

    UOverlay->NewFrame();
    UOverlay->Update(UDevice, fDeltaTime);

    UHLInstance->BeginRender(commandBuffer);

    UVBO->Bind(commandBuffer);

    if (m_pSkybox)
        m_pSkybox->Render(commandBuffer, currentFrame);

    m_pRoot->Render(commandBuffer, currentFrame);

    // Imgui overlays (Demo)
    UOverlay->DrawFrame(UDevice, commandBuffer, currentFrame);

    UHLInstance->EndRender(commandBuffer);

    UHLInstance->EndFrame(commandBuffer, &bResult);
    if (!bResult)
    {
        ReCreate();
    }
}