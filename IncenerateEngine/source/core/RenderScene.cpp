#include "RenderScene.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "Core/VulkanAllocator.h"
#include "KeyMapping/InputMapper.h"
#include "Resources/ResourceManager.h"
#include "Core/Overlay/ImguiOverlay.h"
#include "Objects/Components/Camera/CameraComponent.h"
#include "Objects/Components/Camera/CameraManager.h"
#include "GlobalVariables.h"

using namespace Engine;
using namespace Engine::Core;
using namespace Engine::Objects::Components;

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
    m_pRoot = std::make_shared<Objects::Components::SceneRootComponent>();
    m_pResourceManager = std::make_shared<Resources::ResourceManager>();
    m_pResourceManager->Create();
    UOverlay->Create(m_pResourceManager);   //TODO: bad!

    //TODO: for test
    for(uint32_t i = 0; i < 40; i++)
    {
        Resources::Light::LightSourceBase light;
        light.position = glm::vec4(frandom(-8.f, 8.f), frandom(-3.f, -1.f), frandom(-8.f, 8.f), 1.f);
        light.color = glm::vec3(frandom(0.f, 1.f), frandom(0.f, 1.f), frandom(0.f, 1.f));
        light.attenuation = frandom(0.f, 10.f);
        vLights.emplace_back(light);
    }
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

void RenderScene::SetEnvironment(std::shared_ptr<Objects::RenderObject> pEnvironment)
{
    m_pEnvironment = pEnvironment;
}

void RenderScene::CreateObjects()
{
    UVBO->Create();
    m_pEnvironment->Create(m_pResourceManager);
    m_pSkybox->Create(m_pResourceManager);
    m_pRoot->Create(m_pResourceManager);
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
    UOverlay->Update(fDeltaTime);

    FLightningData ubo;
    auto camera = CameraManager::GetInstance()->GetCurrentCamera();
    for(uint32_t i = 0; i < vLights.size(); i++)
    {
        ubo.lights[i].position = vLights.at(i).position;
        ubo.lights[i].color = vLights.at(i).color;
        ubo.lights[i].radius = vLights.at(i).attenuation;
    }
    ubo.lightCount = vLights.size();
    ubo.viewPos = glm::vec4(camera->GetTransform().pos, 1.0);
    ubo.ambient = GlobalVariables::ambientLight;
    ubo.tone = GlobalVariables::postprocessTone;
    ubo.gamma = GlobalVariables::postprocessGamma;
    ULightUniform->UpdateUniformBuffer(currentFrame, &ubo);

    UVBO->Bind(commandBuffer);
    //Main render
    URenderer->BeginRender(commandBuffer);

    if(m_pSkybox)
        m_pSkybox->Render(commandBuffer, currentFrame);

    m_pRoot->Render(commandBuffer, currentFrame);

    URenderer->EndRender(commandBuffer);

    //Post processing
    URenderer->BeginPostProcess(commandBuffer);

    URenderer->EndPostProcess(commandBuffer);

    UHLInstance->EndFrame(commandBuffer, &bResult);
    if (!bResult)
    {
        ReCreate();
    }
}