#include "VulkanHighLevel.h"
#include "Core/Image/ImageLoader.h"
#include "VulkanStaticHelper.h"
#include "Rendering/DeferredRenderer.h"
#include "Rendering/PBRCompositionRenderer.h"
#include "Rendering/FinalCompositionRenderer.h"
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
    CreateCommandBuffers();
}

void RenderSystem::ReCreate()
{
    m_ScreenExtent = USwapChain->GetExtent();
    m_pStages->ReCreate(USwapChain->GetFramesInFlight());
    CreateCommandBuffers();
}

void RenderSystem::Cleanup()
{
    m_pStages->Cleanup();
    UDevice->Destroy(m_vCommandBuffers);
}

vk::CommandBuffer RenderSystem::BeginFrame()
{
    assert(!m_bFrameStarted && "Can't call beginFrame while already in progress");

    USwapChain->AcquireNextImage(&m_iImageIndex);

    m_bFrameStarted = true;

    auto commandBuffer = GetCurrentCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    commandBuffer.begin(beginInfo);
    return commandBuffer;
}

vk::Result RenderSystem::EndFrame()
{
    assert(m_bFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();

    commandBuffer.end();
    m_bFrameStarted = false;
    return USwapChain->SubmitCommandBuffers(&commandBuffer, &m_iImageIndex);
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

void RenderSystem::CreateCommandBuffers()
{
    assert(UDevice && "Cannot create command buffers, cause logical device is not valid.");
    assert(USwapChain && "Cannot create command buffers, cause swap chain is not valid.");
    m_vCommandBuffers.resize(USwapChain->GetFramebuffers().size());

    m_vCommandBuffers = UDevice->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)m_vCommandBuffers.size());
    assert(!m_vCommandBuffers.empty() && "Created command buffers is not valid.");
}

vk::CommandBuffer RenderSystem::GetCurrentCommandBuffer() const
{
    assert(m_bFrameStarted && "Cannot get command buffer when frame not in progress");
    return m_vCommandBuffers[m_iImageIndex];
}