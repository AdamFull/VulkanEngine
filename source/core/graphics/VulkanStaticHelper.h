#pragma once
#include "data_types/VulkanQueueFamilies.h"
#include "data_types/VulkanSwapChainSipportDetails.h"

namespace Engine
{
    namespace Core
    {
        class VulkanStaticHelper
        {
        public:
            static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);
            static std::vector<const char *> GetRequiredExtensions(bool validation = false);

            static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

            static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &);
            static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

            static uint32_t VkFormatToGLFormat(vk::Format format);
            static vk::Format GLFormatToVkFormat(uint32_t format);

            static bool HasStencilComponent(vk::Format format);
        };
    }
}