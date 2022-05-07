#include "CommandBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CCommandBuffer::CCommandBuffer(bool _begin, vk::QueueFlagBits queueType, vk::CommandBufferLevel bufferLevel, uint32_t count) :
commandPool(CDevice::inst()->getCommandPool()), queueType(queueType)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = count;

    // TODO: Handle error
    vCommandBuffers = CDevice::inst()->getLogical().allocateCommandBuffers(allocInfo);

    if(_begin)
        begin();
}

void CCommandBuffer::cleanup()
{
    if(!bIsClean)
    {
        CDevice::inst()->getLogical().freeCommandBuffers(commandPool->getCommandPool(), vCommandBuffers);
        vCommandBuffers.clear();
        bIsClean = true;
    }
}

CCommandBuffer::~CCommandBuffer()
{
    cleanup();
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

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::Fence fence = CDevice::inst()->make<vk::Fence, vk::FenceCreateInfo>(fenceCreateInfo);
    CDevice::inst()->getLogical().resetFences(1, &fence);

    vk::Queue queue{};
    switch (queueType)
    {
    case vk::QueueFlagBits::eGraphics: {
        queue = CDevice::inst()->getGraphicsQueue();
    } break;
    case vk::QueueFlagBits::eCompute: {
        queue = CDevice::inst()->getComputeQueue();
    } break;
    case vk::QueueFlagBits::eTransfer: {
        queue = CDevice::inst()->getTransferQueue();
    } break;
    }
    queue.submit(submitInfo, fence);
    auto fencesResult = CDevice::inst()->getLogical().waitForFences(1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    CDevice::inst()->destroy(fence);
    //queue.waitIdle();
}

vk::Result CCommandBuffer::submit(uint32_t& imageIndex)
{
    if (running)
		end();

    auto commandBuffer = getCommandBuffer();
    return CSwapChain::inst()->submitCommandBuffers(&commandBuffer, &imageIndex, queueType);
}