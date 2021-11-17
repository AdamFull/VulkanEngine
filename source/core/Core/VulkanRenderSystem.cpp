#include "VulkanRenderSystem.h"
#include "Resources/Textures/ImageLoader.h"
#include "VulkanStaticHelper.h"
#include "VulkanAllocator.h"
#include "VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Loaders;

RenderSystem::RenderSystem(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapchain) :
m_device(device),
m_swapchain(swapchain)
{

}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Create()
{
    screenExtent = m_swapchain->GetExtent();

    CreateCommandBuffers();
}

void RenderSystem::ReCreate()
{
    screenExtent = m_swapchain->GetExtent();
    //???
    CreateCommandBuffers();
}

void RenderSystem::Cleanup()
{
    m_device->Destroy(data.vCommandBuffers);
}

vk::CommandBuffer RenderSystem::BeginFrame()
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

vk::Result RenderSystem::EndFrame()
{
    assert(data.bFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = GetCurrentCommandBuffer();

    commandBuffer.end();
    data.bFrameStarted = false;
    return m_swapchain->SubmitCommandBuffers(&commandBuffer, &data.imageIndex);
}

void RenderSystem::BeginRender(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    std::vector<vk::ClearValue> clearValues{};
    for(auto& [attachment, param] : m_swapchain->vAttachments)
    {
        vk::ClearValue clearValue{};
        clearValue.color = param.color;
        clearValues.emplace_back(clearValue);
    }

    //Clear depth
    vk::ClearValue clearValue{};
    clearValue.depthStencil = vk::ClearDepthStencilValue{1.f, 0};
    clearValues.emplace_back(clearValue);

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = m_swapchain->GetOffscreenRenderPass();
    renderPassInfo.framebuffer = m_swapchain->GetOffscreenFramebuffers().at(data.imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = screenExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport = Initializers::Viewport(screenExtent.width, screenExtent.height);
    vk::Rect2D scissor{{0, 0}, screenExtent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void RenderSystem::EndRender(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    commandBuffer.endRenderPass();
}

void RenderSystem::BeginPostProcess(vk::CommandBuffer& commandBuffer)
{
    assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = m_swapchain->GetRenderPass();
    renderPassInfo.framebuffer = m_swapchain->GetFramebuffers().at(data.imageIndex);
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
    m_swapchain->UpdateCompositionMaterial(commandBuffer);
    commandBuffer.draw(3, 1, 0, 0);

    UOverlay->DrawFrame(commandBuffer, data.imageIndex);

    assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    commandBuffer.endRenderPass();
}

void RenderSystem::CreateCommandBuffers()
{
    assert(m_device && "Cannot create command buffers, cause logical device is not valid.");
    assert(m_swapchain && "Cannot create command buffers, cause swap chain is not valid.");
    data.vCommandBuffers.resize(m_swapchain->GetFramebuffers().size());

    data.vCommandBuffers = m_device->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)data.vCommandBuffers.size());
    assert(!data.vCommandBuffers.empty() && "Created command buffers is not valid.");
}

vk::CommandBuffer RenderSystem::GetCurrentCommandBuffer() const
{
    assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
    return data.vCommandBuffers[data.imageIndex];
}