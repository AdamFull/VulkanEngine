#include "DeferredRenderer.h"
#include "Core/Image/Image.h"
#include "Resources/Materials/MaterialDeferred.h"
#include "Resources/ResourceManager.h"
#include "Core/Scene/Objects/RenderObject.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;

void DeferredRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    m_eRendererType = renderer_type_t::eDeferredPBR;

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
        },
        {
            ETextureAttachmentType::eLightningMask,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eNormal,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eDiffuseAlbedo,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR8G8B8A8Snorm,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eEmissive,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR8G8B8A8Snorm,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eMRAH,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR8G8B8A8Snorm,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    out_extent = vk::Extent2D{800, 600};//USwapChain->GetExtent();
    out_extent = vk::Extent2D(out_extent.width, out_extent.height);
    RendererBase::Create(pResMgr);
}

void DeferredRenderer::ReCreate(uint32_t framesInFlight)
{
    out_extent = USwapChain->GetExtent();
    out_extent = vk::Extent2D(out_extent.width, out_extent.height);
}

void DeferredRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    auto imageIndex = USwapChain->GetCurrentFrame();
    UVBO->Bind(commandBuffer);
    BeginRender(commandBuffer);
    m_pRenderNode->Render(commandBuffer, imageIndex);
    EndRender(commandBuffer);

    RendererBase::Render(commandBuffer);
}

void DeferredRenderer::Cleanup()
{
    for (auto framebuffer : m_vFramebuffers)
        UDevice->Destroy(framebuffer);
    
    UDevice->Destroy(m_RenderPass);
    UDevice->Destroy(m_Sampler);
}