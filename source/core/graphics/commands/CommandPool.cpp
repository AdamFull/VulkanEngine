#include "CommandPool.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CCommandPool::CCommandPool(const std::thread::id &threadId) : threadId(threadId) 
{
    //TODO: refactor here device calls
	QueueFamilyIndices queueFamilyIndices = CDevice::inst()->findQueueFamilies();

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vk::Result res = CDevice::inst()->create(poolInfo, &commandPool);
    assert(res == vk::Result::eSuccess && "Cannot create command pool.");
}

void CCommandPool::cleanup()
{
    if(!bIsClean)
    {
        CDevice::inst()->destroy(&commandPool);
        bIsClean = true;
    }
}

CCommandPool::~CCommandPool() 
{
	cleanup();
}