#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "Pipeline/GraphicsPipeline.h"
#include "VulkanUniform.h"
#include "VulkanSwapChain.h"

using namespace Engine::Core;

Renderer::Renderer(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain) :
m_device(device),
m_swapchain(swapchain)
{

}

Renderer::~Renderer()
{
    Cleanup();
}

void Renderer::Create()
{
    CreateCommandBuffers();
}

void Renderer::ReCreate()
{
    CreateCommandBuffers();
}

void Renderer::Cleanup()
{
    m_device->Destroy(data.vCommandBuffers);
}

vk::CommandBuffer Renderer::BeginFrame()
{
    assert(!data.bFrameStarted && "Can't call beginFrame while already in progress");

    m_swapchain->AcquireNextImage(&data.imageIndex);

    data.bFrameStarted = true;

    auto commandBuffer = GetCurrentCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    commandBuffer.begin(beginInfo);
    return commandBuffer;
}

vk::Result Renderer::EndFrame()
{
    assert(data.bFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();

    commandBuffer.end();
    data.bFrameStarted = false;
    return m_swapchain->SubmitCommandBuffers(&commandBuffer, &data.imageIndex);
}

void Renderer::BeginRender(vk::CommandBuffer commandBuffer)
{
    assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = m_swapchain->GetRenderPass();
    renderPassInfo.framebuffer = m_swapchain->GetFramebuffers().at(data.imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = m_swapchain->GetExtent();

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain->GetExtent().width);
    viewport.height = static_cast<float>(m_swapchain->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vk::Rect2D scissor{{0, 0}, m_swapchain->GetExtent()};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::EndRender(vk::CommandBuffer commandBuffer)
{
    assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    commandBuffer.endRenderPass();
}

void Renderer::CreateCommandBuffers()
{
    assert(m_device && "Cannot create command buffers, cause logical device is not valid.");
    assert(m_swapchain && "Cannot create command buffers, cause swap chain is not valid.");
    data.vCommandBuffers.resize(m_swapchain->GetFramebuffers().size());

    data.vCommandBuffers = m_device->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)data.vCommandBuffers.size());
    assert(!data.vCommandBuffers.empty() && "Created command buffers is not valid.");
}

vk::CommandBuffer Renderer::GetCurrentCommandBuffer() const
{
    assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
    return data.vCommandBuffers[data.imageIndex];
}