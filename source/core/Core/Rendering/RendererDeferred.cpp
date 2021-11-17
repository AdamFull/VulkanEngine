#include "RendererDeferred.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Core;
using namespace Engine::Core::Renderer;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

RendererDeferred::RendererDeferred(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void RendererDeferred::Create()
{
    vAttachments = 
    {
        {
            ETextureAttachmentType::ePosition,
            FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageAspectFlagBits::eColor,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eLightningMask,
            FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageAspectFlagBits::eColor,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eNormal,
            FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageAspectFlagBits::eColor,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eDiffuseAlbedo,
            FAttachmentInfo
            (
                vk::Format::eR8G8B8A8Snorm,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageAspectFlagBits::eColor,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    RendererBase::Create();
}

void RendererDeferred::ReCreate()
{
    RendererBase::ReCreate();
}

void RendererDeferred::Update(vk::CommandBuffer& commandBuffer, void* ubo)
{
    RendererBase::Update(commandBuffer, ubo);
}

void RendererDeferred::Cleanup()
{
    RendererBase::Cleanup();
}


void RendererDeferred::CreateRenderPass()
{
    RendererBase::CreateRenderPass();
}

void RendererDeferred::CreateFrameBuffers()
{
    RendererBase::CreateFrameBuffers();
    
}

void RendererDeferred::CreateUniformBuffers()
{
    RendererBase::CreateUniformBuffers();
    auto framesInFlight = m_swapchain->GetFramesInFlight();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));
}

void RendererDeferred::CreateMaterial()
{
    RendererBase::CreateMaterial();

    m_pMaterial = FDefaultAllocator::Allocate<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
}
