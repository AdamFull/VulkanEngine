#include "RenderScene.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/VulkanHighLevel.h"
#include "keymapper/InputMapper.h"
#include "graphics/overlay/ImguiOverlay.h"

#include "graphics/rendering/RendererBase.h"
#include "resources/ResourceCunstruct.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

RenderScene::~RenderScene()
{
    Destroy();
}

void RenderScene::Create()
{
    m_pRoot = std::make_shared<Core::Scene::Objects::Components::SceneRootComponent>();
    m_pResourceManager = std::make_shared<Resources::ResourceManager>();
    m_pResourceManager->Create();
    UOverlay->Create(m_pResourceManager, m_pRoot);   //TODO: bad!
}

void RenderScene::ReCreate()
{
    m_pRoot->Cleanup();

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

    m_pRoot->Destroy();
}

void RenderScene::AttachObject(std::shared_ptr<Core::Scene::Objects::RenderObject> object)
{
    object->SetParent(m_pRoot);
}

void RenderScene::CreateObjects()
{
    UVBO->Create();
    
    /*auto pShadowRenderer = URenderer->PushStage(FRendererCreateInfo::ERendererType::eShadow, vk::Extent2D{});
    pShadowRenderer->Create(m_pResourceManager);
    pShadowRenderer->SetRenderNode(m_pRoot);*/

    auto pDeferredRenderer = URenderer->PushStage(FRendererCreateInfo::ERendererType::eDeferredPBR, vk::Extent2D{});
    pDeferredRenderer->Create(m_pResourceManager);
    pDeferredRenderer->SetRenderNode(m_pRoot);

    auto pComposition = URenderer->PushStage(FRendererCreateInfo::ERendererType::ePBRComposition, vk::Extent2D{});
    pComposition->Create(m_pResourceManager);

    auto pScreenRender = URenderer->PushStage(FRendererCreateInfo::ERendererType::ePostProcess, vk::Extent2D{});
    pScreenRender->Create(m_pResourceManager);

    m_pRoot->Create(m_pResourceManager);
}

void RenderScene::Render(float fDeltaTime)
{
    m_pRoot->Update(fDeltaTime);

    bool bResult;
    auto commandBuffer = UHLInstance->BeginFrame(&bResult);
    if (!bResult)
        ReCreate();

    uint32_t currentFrame = URenderer->GetImageIndex();

    UOverlay->NewFrame();
    UOverlay->Update(fDeltaTime);

    URenderer->Render(commandBuffer);
    
    UHLInstance->EndFrame(commandBuffer, &bResult);
    if (!bResult)
        ReCreate();
}