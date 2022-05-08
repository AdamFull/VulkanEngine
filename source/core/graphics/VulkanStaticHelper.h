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
            static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
            static std::vector<const char *> getRequiredExtensions(bool validation = false);

            static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

            static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &);
            static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

            static uint32_t vkFormatToGLFormat(vk::Format format);
            static vk::Format GLFormatToVkFormat(uint32_t format);

            static bool hasStencilComponent(vk::Format format);
        };
    }
}