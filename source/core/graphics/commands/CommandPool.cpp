#include "CommandPool.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CommandPool::CommandPool(const std::thread::id &threadId) : threadId(threadId) 
{
    auto& logical = UDevice->GetLogical();
	QueueFamilyIndices queueFamilyIndices = UDevice->FindQueueFamilies(UDevice->GetPhysical());

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    commandPool = logical.createCommandPool(poolInfo);
}

CommandPool::~CommandPool() 
{
	UDevice->Destroy(commandPool);
}