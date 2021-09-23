#pragma once

namespace Engine::Device
{
    struct FVulkanDevice
    {
        vk::UniqueInstance vkInstance;
        VkDebugUtilsMessengerEXT debugMessenger;
        vk::PhysicalDevice physical;
        vk::CommandPool commandPool;

        vk::UniqueDevice logical;
        vk::SurfaceKHR surface;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1
    };
}
