#include "VulkanHighLevel.h"
#include "graphics/image/ImageLoader.h"
#include "VulkanStaticHelper.h"
#include "rendering/DeferredRenderer.h"
#include "rendering/PBRCompositionRenderer.h"
#include "rendering/FinalCompositionRenderer.h"
#include "VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Loaders;

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Create()
{
    m_ScreenExtent = USwapChain->GetExtent();
    m_pCommandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
}

void RenderSystem::ReCreate()
{
    m_ScreenExtent = USwapChain->GetExtent();
    m_pStages->ReCreate(USwapChain->GetFramesInFlight());
    m_pCommandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
}

void RenderSystem::Cleanup()
{
    m_pStages->Cleanup();
}

vk::CommandBuffer RenderSystem::BeginFrame()
{
    assert(!m_bFrameStarted && "Can't call beginFrame while already in progress");

    USwapChain->AcquireNextImage(&m_iImageIndex);

    m_bFrameStarted = true;

    m_pCommandBuffers->begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse, m_iImageIndex);
    return m_pCommandBuffers->GetCommandBuffer();
}

vk::Result RenderSystem::EndFrame()
{
    assert(m_bFrameStarted && "Can't call endFrame while frame is not in progress");
    m_pCommandBuffers->end();
    m_bFrameStarted = false;
    return m_pCommandBuffers->submit(m_iImageIndex);
}

void RenderSystem::Render(vk::CommandBuffer& commandBuffer)
{
    m_pStages->Render(commandBuffer);
}

std::shared_ptr<Rendering::RendererBase> RenderSystem::PushStage(FRendererCreateInfo::ERendererType eType, vk::Extent2D extent)
{
    //TODO: create stage factory
    std::shared_ptr<Rendering::RendererBase> pNewRenderer;

    switch (eType)
    {
    /*case FRendererCreateInfo::ERendererType::eShadow:
        pNewRenderer = std::make_shared<Rendering::ShadowRenderer>();
        break;*/
    case FRendererCreateInfo::ERendererType::eDeferredPBR:
        pNewRenderer = std::make_shared<Rendering::DeferredRenderer>();
        break;
    case FRendererCreateInfo::ERendererType::ePBRComposition:
        pNewRenderer = std::make_shared<Rendering::PBRCompositionRenderer>();
        break;
    case FRendererCreateInfo::ERendererType::ePostProcess:
        pNewRenderer = std::make_shared<Rendering::FinalCompositionRenderer>();
        break;
    default:
        break;
    }

    if(!m_pStages)
    {
        m_pStages = pNewRenderer;
        return pNewRenderer;
    }
    
    m_pStages->SetNextStage(pNewRenderer);
    return pNewRenderer;
}

vk::CommandBuffer RenderSystem::GetCurrentCommandBuffer() const
{
    assert(m_bFrameStarted && "Cannot get command buffer when frame not in progress");
    return m_pCommandBuffers->GetCommandBuffer();
}