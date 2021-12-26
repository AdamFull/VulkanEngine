#include "FinalCompositionRenderer.h"
#include "Resources/Textures/Image.h"
#include "Resources/Materials/MaterialDeferred.h"
#include "Resources/ResourceManager.h"
#include "Core/Scene/Objects/RenderObject.h"
#include "Core/Scene/Objects/Components/Camera/CameraManager.h"
#include "Core/Scene/Lightning/LightSourceManager.h"
#include "Core/Scene/Objects/Components/Light/LightComponent.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Core::Scene::Objects::Components::Light;

void FinalCompositionRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));

    m_eRendererType = renderer_type_t::eFinalize;

    m_vColorAttachments = 
    {
        {
            ETextureAttachmentType::ePosition,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    out_extent = USwapChain->GetExtent();
    RendererBase::Create(pResMgr);
    CreateMaterial(pResMgr);
}

void FinalCompositionRenderer::ReCreate(uint32_t framesInFlight)
{
    out_extent = USwapChain->GetExtent();
    RendererBase::ReCreate(framesInFlight);
    m_pMaterial->ReCreate();
}

void FinalCompositionRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    BeginRender(commandBuffer);
    auto imageIndex = USwapChain->GetCurrentFrame();
    auto& mImages = m_pPrev->GetProducts();
    for(auto& [attachment, texture] : mImages)
        m_pMaterial->AddTexture(attachment, texture);

    //May be move to CompositionObject
    FLightningData ubo;
    auto camera = CameraManager::getInstance()->GetCurrentCamera();
    auto vLights = LightSourceManager::getInstance()->GetSources();
    for(std::size_t i = 0; i < vLights.size(); i++)
        ubo.lights[i] = vLights.at(i);

    ubo.lightCount = vLights.size();
    ubo.viewPos = camera->viewPos; //camera->viewPos
    ubo.gamma = GlobalVariables::postprocessGamma;
    ubo.exposure = GlobalVariables::postprocessExposure;
    
    m_pUniform->UpdateUniformBuffer(imageIndex, &ubo);
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    m_pMaterial->Update(buffer->GetDscriptor(), imageIndex);
    m_pMaterial->Bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);
    UOverlay->DrawFrame(commandBuffer, imageIndex);

    EndRender(commandBuffer);

    RendererBase::Render(commandBuffer);
}

void FinalCompositionRenderer::Cleanup()
{
    RendererBase::Cleanup();
}

void FinalCompositionRenderer::CreateRenderPass()
{
    m_RenderPass = USwapChain->GetRenderPass();
}

void FinalCompositionRenderer::CreateFramebuffers()
{
    m_vFramebuffers = USwapChain->GetFramebuffers();
}

void FinalCompositionRenderer::CreateMaterial(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pMaterial = std::make_shared<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
    m_pMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, pResMgr->Get<Image>("environment_component_brdf"));
    m_pMaterial->AddTexture(ETextureAttachmentType::eIrradiance, pResMgr->Get<Image>("environment_component_irradiate_cube"));
    m_pMaterial->AddTexture(ETextureAttachmentType::ePrefiltred, pResMgr->Get<Image>("environment_component_prefiltred_cube"));
}