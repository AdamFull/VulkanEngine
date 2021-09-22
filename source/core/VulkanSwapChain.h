#pragma once
#include "DataTypes/VulkanImage.hpp"
#include "VulkanMainStructures.h"

namespace Engine::SwapChain
{

        vk::Format FindDepthFormat();

        vk::Result AcquireNextImage(uint32_t *imageIndex);
        vk::Result SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        
}