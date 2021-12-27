#include "RenderScene.h"
#include "Core/Scene/Objects/Components/Camera/CameraManager.h"
#include "Core/VulkanHighLevel.h"
#include "KeyMapping/InputMapper.h"
#include "Core/Overlay/ImguiOverlay.h"

#include "Core/Rendering/RendererBase.h"
#include "Resources/ResourceCunstruct.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Resources;

float frandom(float min, float max) 
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

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

    //TODO: for test
    //for(uint32_t i = 0; i < 10; i++)
    //{
    //    LightSourceManager::GetInstance()->CreateSource(ELightSourceType::ePoint,
    //    glm::vec3(frandom(-8.f, 8.f), frandom(-3.f, -1.f), frandom(-8.f, 8.f)),
    //    glm::vec3(frandom(0.f, 1.f), frandom(0.f, 1.f), frandom(0.f, 1.f)),
    //    frandom(10.f, 30.f));
    //}
    //LightSourceManager::getInstance()->CreateSource(ELightSourceType::ePoint, glm::vec3(7.5, -2.7, 3.8f), glm::vec3(1.0f, 0.0, 0.0), 5.f);
    //LightSourceManager::getInstance()->CreateSource(ELightSourceType::ePoint, glm::vec3(-7.7, -2.7, 3.8f), glm::vec3(0.0f, 1.0, 0.0), 5.f);
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

void RenderScene::SetEnvironment(std::shared_ptr<Core::Scene::Objects::RenderObject> pEnvironment)
{
    m_pEnvironment = pEnvironment;
}

void RenderScene::CreateObjects()
{
    UVBO->Create();
    m_pEnvironment->Create(m_pResourceManager);
    
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