#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "Pipeline/GraphicsPipelineDiffuse.h"
#include "WindowHandle.h"
#include "VulkanUniform.h"
#include "VulkanSwapChain.h"

namespace Engine
{
    void Renderer::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        CreateCommandBuffers(device, swapchain);
    }

    void Renderer::ReCreate(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        device->Destroy(data.vCommandBuffers);
        CreateCommandBuffers(device, swapchain);
    }

    vk::CommandBuffer Renderer::BeginFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(!data.bFrameStarted && "Can't call beginFrame while already in progress");

        swapchain->AcquireNextImage(device, &data.imageIndex);

        data.bFrameStarted = true;

        auto commandBuffer = GetCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

        commandBuffer.begin(beginInfo);
        return commandBuffer;
    }

    vk::Result Renderer::EndFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(data.bFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = GetCurrentCommandBuffer();

        commandBuffer.end();

        return swapchain->SubmitCommandBuffers(device, &commandBuffer, &data.imageIndex);
    }

    void Renderer::BeginRender(vk::CommandBuffer commandBuffer, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(data.bFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        vk::RenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass = swapchain->GetRenderPass();
        renderPassInfo.framebuffer = swapchain->GetFramebuffers().at(data.imageIndex);
        renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassInfo.renderArea.extent =  swapchain->GetExtent();

        std::array<vk::ClearValue, 2> clearValues{};
        clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    }

    void Renderer::EndRender(vk::CommandBuffer commandBuffer, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
        commandBuffer.endRenderPass();
    }

    void Renderer::CreateCommandBuffers(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        assert(device && "Cannot create command buffers, cause logical device is not valid.");
        assert(swapchain && "Cannot create command buffers, cause swap chain is not valid.");
        data.vCommandBuffers.resize(swapchain->GetFramebuffers().size());

        data.vCommandBuffers = device->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)data.vCommandBuffers.size());
        assert(!data.vCommandBuffers.empty() && "Created command buffers is not valid.");
    }

    vk::CommandBuffer Renderer::GetCurrentCommandBuffer() const 
    {
        assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
        return data.vCommandBuffers[data.imageIndex];
    }
}