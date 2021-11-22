#include "DeferredRenderer.h"
#include "Resources/Textures/Image.h"
#include "Resources/Materials/MaterialDeferred.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

void DeferredRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
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

    m_DepthAttachment = 
    FRendererCreateInfo::FAttachmentInfo
    (
        vk::Format::eR32Sfloat,
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
    );

    RendererBase::Create(pResMgr);
}

void DeferredRenderer::CreateMaterial(std::shared_ptr<ResourceManager> pResMgr)
{
    RendererBase::CreateMaterial(pResMgr);

    m_pMaterial = std::make_shared<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
    m_pMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, pResMgr->Get<Image>("environment_component_brdf"));
    m_pMaterial->AddTexture(ETextureAttachmentType::eIrradiance, pResMgr->Get<Image>("environment_component_irradiate_cube"));
    m_pMaterial->AddTexture(ETextureAttachmentType::ePrefiltred, pResMgr->Get<Image>("environment_component_prefiltred_cube"));
}