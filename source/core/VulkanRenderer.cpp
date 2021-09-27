#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "Pipeline/GraphicsPipeline.h"
#include "WindowHandle.h"
#include "VulkanUniform.h"

namespace Engine
{
    void Renderer::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        CreateDescriptorSets(device, swapchain, pipeline, uniform);
        CreateCommandBuffers(device, swapchain, pipeline, uniform);
    }

    vk::CommandBuffer Renderer::BeginFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        assert(!data.bFrameStarted && "Can't call beginFrame while already in progress");

        try { swapchain->AcquireNextImage(device, &data.imageIndex); }
        catch (vk::OutOfDateKHRError err)
        {
            RecreateSwapChain(device, swapchain, pipeline, uniform);
            return nullptr;
        }
        catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

        data.bFrameStarted = true;

        auto commandBuffer = GetCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

        commandBuffer.begin(beginInfo);
        return commandBuffer;
    }

    void Renderer::EndFrame(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        assert(data.bFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = GetCurrentCommandBuffer();

        commandBuffer.end();

        vk::Result resultPresent;
        try
        {
            resultPresent = swapchain->SubmitCommandBuffers(device, &commandBuffer, &data.imageIndex);
        }
        catch (vk::OutOfDateKHRError err)
        {
            resultPresent = vk::Result::eErrorOutOfDateKHR;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR || WindowHandle::m_bWasResized)
        {
            WindowHandle::m_bWasResized = false;
            RecreateSwapChain(device, swapchain, pipeline, uniform);
            return;
        }
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

    void Renderer::EndRender(vk::CommandBuffer commandBuffer)
    {
        assert(data.bFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
        commandBuffer.endRenderPass();
    }

    void Renderer::RecreateSwapChain(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        device->GPUWait();

        CleanupSwapChain(device, swapchain);
        swapchain->ReCreate(device);
        
        pipeline->RecreatePipeline(device, swapchain);

        CreateCommandBuffers(device, swapchain, pipeline, uniform);
    }

    void Renderer::CleanupSwapChain(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        swapchain->Cleanup(device);
        device->Destroy(data.vCommandBuffers);
    }

     void Renderer::CreateDescriptorSets(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        uint32_t images = swapchain->GetImages().size();
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, pipeline->GetDescriptorSetLayout());

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = pipeline->GetDescriptorPool();
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        data.vDescriptorSets.resize(images);

        auto result = device->GetLogical()->allocateDescriptorSets(&allocInfo, data.vDescriptorSets.data());

        for (size_t i = 0; i < images; i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniform->GetUniformBuffers().at(i);
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(FUniformData);

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].dstSet = data.vDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].dstSet = data.vDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &m_pTexture->descriptor;

            device->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void Renderer::CreateCommandBuffers(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain, std::unique_ptr<GraphicsPipeline>& pipeline, std::unique_ptr<UniformBuffer>& uniform)
    {
        assert(device && "Cannot create command buffers, cause logical device is not valid.");
        assert(swapchain && "Cannot create command buffers, cause swap chain is not valid.");
        assert(pipeline && "Cannot create command buffers, cause pipeline is not valid.");
        data.vCommandBuffers.resize(swapchain->GetFramebuffers().size());

        data.vCommandBuffers = device->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)data.vCommandBuffers.size());
        assert(!data.vCommandBuffers.empty() && "Created command buffers is not valid.");
    }

    vk::CommandBuffer Renderer::GetCurrentCommandBuffer() const 
    {
        assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
        return data.vCommandBuffers[data.imageIndex];
    }

    vk::DescriptorSet Renderer::GetCurrentDescriptorSets() const
    {
        assert(data.bFrameStarted && "Cannot get command buffer when frame not in progress");
        return data.vDescriptorSets[data.imageIndex];
    }
}