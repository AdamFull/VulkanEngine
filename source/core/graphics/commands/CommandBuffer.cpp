#include "CommandBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CCommandBuffer::CCommandBuffer(bool _begin, vk::QueueFlagBits queueType, vk::CommandBufferLevel bufferLevel, uint32_t count) :
commandPool(UDevice->getCommandPool()), queueType(queueType)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = count;

    // TODO: Handle error
    vCommandBuffers = UDevice->getLogical().allocateCommandBuffers(allocInfo);

    if(_begin)
        begin();
}

CCommandBuffer::~CCommandBuffer()
{
    UDevice->getLogical().freeCommandBuffers(commandPool->getCommandPool(), vCommandBuffers);
    vCommandBuffers.clear();
}


void CCommandBuffer::begin(vk::CommandBufferUsageFlags usage, uint32_t index)
{
    if (running)
		return;

    frameIndex = index;
    vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = usage;
    auto commandBuffer = getCommandBuffer();
	commandBuffer.begin(beginInfo);
	running = true;
}

void CCommandBuffer::end()
{
    if (!running) return;

    auto commandBuffer = getCommandBuffer();
    commandBuffer.end();

    running = false;
}


void CCommandBuffer::submitIdle()
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
        queue = UDevice->getGraphicsQueue();
    } break;
    case vk::QueueFlagBits::eCompute: {
        queue = UDevice->getComputeQueue();
    } break;
    case vk::QueueFlagBits::eTransfer: {
        queue = UDevice->getTransferQueue();
    } break;
    }
    queue.submit(submitInfo, nullptr);
    queue.waitIdle();
}

vk::Result CCommandBuffer::submit(uint32_t& imageIndex)
{
    if (running)
		end();

    auto commandBuffer = getCommandBuffer();
    return USwapChain->submitCommandBuffers(&commandBuffer, &imageIndex, queueType);
}