#pragma once

namespace Engine
{
    namespace Device
    {
        struct FVulkanDevice
        {
            vk::UniqueInstance vkInstance;
            VkDebugUtilsMessengerEXT debugMessenger;
            vk::PhysicalDevice physical;
            vk::CommandPool commandPool;
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffer;

            vk::UniqueDevice logical;
            vk::SurfaceKHR surface;
            vk::Queue graphicsQueue;
            vk::Queue presentQueue;

            vk::SampleCountFlagBits samples{vk::SampleCountFlagBits::e1};
        };
    }
}
