#include "ShadowRenderer.h"
#include "Core/Image/Image.h"
#include "Resources/Materials/MaterialShadow.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;

void ShadowRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    //m_eRendererType = renderer_type_t::eShadow;

    m_vColorAttachments = 
    {
        {
            ETextureAttachmentType::ePosition,
            FRendererCreateInfo::FAttachmentInfo
            (
                Image::GetDepthFormat(),
                vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    out_extent = vk::Extent2D{2048, 2048};
    RendererBase::Create(pResMgr);
    CreateMaterial(pResMgr);
}

void ShadowRenderer::ReCreate(uint32_t framesInFlight)
{
    out_extent = vk::Extent2D{2048, 2048};
}

void ShadowRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    BeginRender(commandBuffer);
    m_pMaterial->Bind(commandBuffer, imageIndex);
    //m_pRenderNode->Render(commandBuffer, imageIndex, false);
    EndRender(commandBuffer);

    RendererBase::Render(commandBuffer);
}

void ShadowRenderer::Cleanup()
{
    for (auto framebuffer : m_vFramebuffers)
        UDevice->Destroy(framebuffer);
    
    UDevice->Destroy(m_RenderPass);
    UDevice->Destroy(m_Sampler);

    RendererBase::Cleanup();
}

void ShadowRenderer::CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    m_pMaterial = std::make_shared<MaterialShadow>();
    m_pMaterial->Create(pResMgr);
}