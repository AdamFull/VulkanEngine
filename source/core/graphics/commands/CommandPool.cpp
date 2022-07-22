#include "CommandPool.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;

CCommandPool::CCommandPool(const std::thread::id &threadId) : threadId(threadId) 
{
    //TODO: refactor here device calls
	QueueFamilyIndices queueFamilyIndices = UDevice->findQueueFamilies();

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vk::Result res = UDevice->create(poolInfo, &commandPool);
    assert(res == vk::Result::eSuccess && "Cannot create command pool.");
}

CCommandPool::~CCommandPool() 
{
	UDevice->destroy(&commandPool);
}