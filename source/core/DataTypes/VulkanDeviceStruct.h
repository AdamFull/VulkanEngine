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

        vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1
    };
}
