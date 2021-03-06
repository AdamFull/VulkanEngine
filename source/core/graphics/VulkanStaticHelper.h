#pragma once
#include "data_types/VulkanQueueFamilies.h"
#include "data_types/VulkanSwapChainSipportDetails.h"

namespace engine
{
    namespace core
    {
        class VulkanStaticHelper
        {
        public:
            static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
            static std::vector<const char *> getRequiredExtensions(bool validation = false);

            static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

            static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &);
            static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

            static bool hasStencilComponent(vk::Format format);
        };
    }
}
