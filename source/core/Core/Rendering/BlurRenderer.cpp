#include "BlurRenderer.h"
#include "Core/Image/Image.h"
#include "Resources/Materials/MaterialBlur.h"
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

void BlurRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FBlurData));

    m_eRendererType = renderer_type_t::eGaussianRender;

    m_vColorAttachments = 
    {
        {
            ETextureAttachmentType::eDiffuseAlbedo,
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

void BlurRenderer::ReCreate(uint32_t framesInFlight)
{
    out_extent = USwapChain->GetExtent();
    RendererBase::ReCreate(framesInFlight);
    m_pMaterial->ReCreate();
}

void BlurRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    BeginRender(commandBuffer);
    auto imageIndex = USwapChain->GetCurrentFrame();
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    auto descriptor = buffer->GetDscriptor();
    m_pMaterial->Update(descriptor, imageIndex);
    m_pMaterial->Bind(commandBuffer, imageIndex);
    commandBuffer.draw(3, 1, 0, 0);

    EndRender(commandBuffer);

    RendererBase::Render(commandBuffer);
}

void BlurRenderer::Cleanup()
{
    RendererBase::Cleanup();
}

void BlurRenderer::AddProduct(ETextureAttachmentType eAttach, std::shared_ptr<Image> image)
{
    m_pMaterial->AddTexture(eAttach, image);
}

void BlurRenderer::SetUniform(void* uniform)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    m_pUniform->UpdateUniformBuffer(imageIndex, uniform);
}

void BlurRenderer::CreateMaterial(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pMaterial = std::make_shared<MaterialBlur>();
    m_pMaterial->Create(nullptr, m_RenderPass);
}