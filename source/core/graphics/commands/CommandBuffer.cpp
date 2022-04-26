#include "CommandBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CommandBuffer::CommandBuffer(bool _begin, vk::QueueFlagBits queueType, vk::CommandBufferLevel bufferLevel, uint32_t count) :
commandPool(UDevice->GetCommandPool()), queueType(queueType)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool->GetCommandPool();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = count;

    // TODO: Handle error
    vCommandBuffers = UDevice->GetLogical().allocateCommandBuffers(allocInfo);

    if(_begin)
        begin();
}

CommandBuffer::~CommandBuffer()
{
    UDevice->GetLogical().freeCommandBuffers(commandPool->GetCommandPool(), vCommandBuffers);
    vCommandBuffers.clear();
}


void CommandBuffer::begin(vk::CommandBufferUsageFlags usage, uint32_t index)
{
    if (running)
		return;

    frameIndex = index;
    vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = usage;
    auto commandBuffer = GetCommandBuffer();
	commandBuffer.begin(beginInfo);
	running = true;
}

void CommandBuffer::end()
{
    if (!running) return;

    auto commandBuffer = GetCommandBuffer();
    commandBuffer.end();

    running = false;
}


void CommandBuffer::submitIdle()
{
    if (running)
		end();

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = vCommandBuffers.size();
    submitInfo.pCommandBuffers = vCommandBuffers.data();

    vk::Queue queue{};
    switch (queueType)
    {
    case vk::QueueFlagBits::eGraphics: {
        queue = UDevice->GetGraphicsQueue();
    } break;
    case vk::QueueFlagBits::eCompute: {
        queue = UDevice->GetComputeQueue();
    } break;
    case vk::QueueFlagBits::eTransfer: {
        queue = UDevice->GetComputeQueue();
    } break;
    }
    queue.submit(submitInfo, nullptr);
    queue.waitIdle();
}

vk::Result CommandBuffer::submit(uint32_t& imageIndex)
{
    if (running)
		end();

    auto commandBuffer = GetCommandBuffer();
    return USwapChain->SubmitCommandBuffers(&commandBuffer, &imageIndex, queueType);
}