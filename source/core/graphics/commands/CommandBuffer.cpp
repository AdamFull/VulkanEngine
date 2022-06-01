#include "CommandBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;

CCommandBuffer::CCommandBuffer(bool _begin, vk::QueueFlagBits queueType, vk::CommandBufferLevel bufferLevel, uint32_t count) :
commandPool(CDevice::inst()->getCommandPool()), queueType(queueType)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = count;

    vCommandBuffers.resize(count);
    vk::Result res = CDevice::inst()->create(allocInfo, vCommandBuffers.data());
    assert(res == vk::Result::eSuccess && "Cannot create bommand buffers");

    if(_begin)
        begin();
}

void CCommandBuffer::cleanup()
{
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to free command buffers, but logical device is invalid");
    if(!bIsClean)
    {
        vkDevice.freeCommandBuffers(commandPool->getCommandPool(), vCommandBuffers);
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


vk::Result CCommandBuffer::submitIdle()
{
    auto& vkDevice = CDevice::inst()->getLogical();
    assert(vkDevice && "Trying to submit queue, but device is invalid.");
    vk::Result res;
    if (running)
		end();

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = vCommandBuffers.size();
    submitInfo.pCommandBuffers = vCommandBuffers.data();

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::Fence fence;
    res = CDevice::inst()->create(fenceCreateInfo, &fence);
    assert(res == vk::Result::eSuccess && "Cannot create fence.");
    res = vkDevice.resetFences(1, &fence);
    assert(res == vk::Result::eSuccess && "Cannot reset fence.");

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
    res = vkDevice.waitForFences(1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    assert(res == vk::Result::eSuccess && "Wait for fences error.");
    CDevice::inst()->destroy(&fence);
    return res;
}

vk::Result CCommandBuffer::submit(uint32_t& imageIndex)
{
    if (running)
		end();

    auto commandBuffer = getCommandBuffer();
    return CDevice::inst()->submitCommandBuffers(&commandBuffer, &imageIndex, queueType);
}