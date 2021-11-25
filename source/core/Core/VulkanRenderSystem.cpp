#include "VulkanHighLevel.h"
#include "Resources/Textures/ImageLoader.h"
#include "VulkanStaticHelper.h"
#include "Rendering/DeferredRenderer.h"
#include "Rendering/FinalCompositionRenderer.h"
#include "VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Loaders;

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Create()
{
    screenExtent = USwapChain->GetExtent();
    CreateCommandBuffers();
}

void RenderSystem::ReCreate()
{
    screenExtent = USwapChain->GetExtent();
    m_pStages->ReCreate(USwapChain->GetFramesInFlight());
    CreateCommandBuffers();
}

void RenderSystem::Cleanup()
{
    m_pStages->Cleanup();
    UDevice->Destroy(data.vCommandBuffers);
}

vk::CommandBuffer RenderSystem::BeginFrame()
{
    assert(!data.bFrameStarted && "Can't call beginFrame while already in progress");

    USwapChain->AcquireNextImage(&data.imageIndex);

    data.bFrameStarted = true;

    auto commandBuffer = GetCurrentCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    commandBuffer.begin(beginInfo);
    return commandBuffer;
}

vk::Result RenderSystem::EndFrame()
{
    assert(data.bFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();

    commandBuffer.end();
    data.bFrameStarted = false;
    return USwapChain->SubmitCommandBuffers(&commandBuffer, &data.imageIndex);
}

void RenderSystem::Render(vk::CommandBuffer& commandBuffer)
{
    m_pStages->Render(commandBuffer);
}

void RenderSystem::PushStage(FRendererCreateInfo::ERendererType eType, vk::Extent2D extent)
{
    //TODO: create stage factory
    std::shared_ptr<Rendering::RendererBase> pNewRenderer;

    switch (eType)
    {
    case FRendererCreateInfo::ERendererType::eDeferredPBR:
        pNewRenderer = std::make_shared<Rendering::DeferredRenderer>();
        break;
    case FRendererCreateInfo::ERendererType::eFinalize:
        pNewRenderer = std::make_shared<Rendering::FinalCompositionRenderer>();
        break;
    }

    if(!m_pStages)
    {
        m_pStages = pNewRenderer;
        return;
    }
    
    m_pStages->SetNextStage(pNewRenderer);
}

/*void RenderSystem::BeginRender(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");
    m_pDeferredRenderer->BeginRender(commandBuffer);
}

void RenderSystem::EndRender(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    m_pDeferredRenderer->EndRender(commandBuffer);
}*/

/*void RenderSystem::BeginPostProcess(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = USwapChain->GetRenderPass();
    renderPassInfo.framebuffer = USwapChain->GetFramebuffers().at(data.imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = screenExtent;

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport = Initializers::Viewport(screenExtent.width, screenExtent.height);
    vk::Rect2D scissor{{0, 0}, screenExtent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void RenderSystem::EndPostProcess(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.draw(3, 1, 0, 0);

    UOverlay->DrawFrame(commandBuffer, data.imageIndex);

    assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    commandBuffer.endRenderPass();
}*/

void RenderSystem::CreateCommandBuffers()
{
    assert(UDevice && "Cannot create command buffers, cause logical device is not valid.");
    assert(USwapChain && "Cannot create command buffers, cause swap chain is not valid.");
    data.vCommandBuffers.resize(USwapChain->GetFramebuffers().size());

    data.vCommandBuffers = UDevice->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)data.vCommandBuffers.size());
    assert(!data.vCommandBuffers.empty() && "Created command buffers is not valid.");
}

vk::CommandBuffer RenderSystem::GetCurrentCommandBuffer() const
{
    assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
    return data.vCommandBuffers[data.imageIndex];
}