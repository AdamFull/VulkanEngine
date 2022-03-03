#include "FinalCompositionRenderer.h"
#include "BlurRenderer.h"
#include "Core/Image/Image.h"
#include "Resources/Materials/MaterialPostProcess.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"
#include "GlobalVariables.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects;

void FinalCompositionRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FPostProcess));

    m_pVertBlur = std::make_shared<BlurRenderer>();
    m_pVertBlur->Create(pResMgr);

    m_pHorizBlur = std::make_shared<BlurRenderer>();
    m_pHorizBlur->Create(pResMgr);

    m_eRendererType = renderer_type_t::ePostProcess;

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
    auto& mImages = m_pPrev->GetProducts();

    FBlurData blur;
    blur.blurScale = GlobalVariables::blurScale;
    blur.blurStrength = GlobalVariables::blurStrength;
    blur.direction = 1;

    m_pHorizBlur->AddProduct(ETextureAttachmentType::eDiffuseAlbedo, m_pPrev->GetProduct(ETextureAttachmentType::eBrightness));
    m_pHorizBlur->SetUniform(&blur);
    m_pHorizBlur->Render(commandBuffer);

    blur.direction = 0;
    m_pVertBlur->AddProduct(ETextureAttachmentType::eDiffuseAlbedo, m_pHorizBlur->GetProduct(ETextureAttachmentType::eDiffuseAlbedo));
    m_pVertBlur->SetUniform(&blur);
    m_pVertBlur->Render(commandBuffer);

    BeginRender(commandBuffer);
    auto imageIndex = USwapChain->GetCurrentFrame();
    
    m_pMaterial->AddTexture(ETextureAttachmentType::eDiffuseAlbedo, m_pPrev->GetProduct(ETextureAttachmentType::eDiffuseAlbedo));
    m_pMaterial->AddTexture(ETextureAttachmentType::eBrightness, m_pVertBlur->GetProduct(ETextureAttachmentType::eDiffuseAlbedo));

    //May be move to CompositionObject
    FPostProcess ubo;
    //HDR
    ubo.gamma = GlobalVariables::postprocessGamma;
    ubo.exposure = GlobalVariables::postprocessExposure;

    m_pUniform->UpdateUniformBuffer(imageIndex, &ubo);
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    auto descriptor = buffer->GetDscriptor();
    m_pMaterial->Update(descriptor, imageIndex);
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
    m_pMaterial = std::make_shared<MaterialPostProcess>();
    m_pMaterial->Create(nullptr);
}