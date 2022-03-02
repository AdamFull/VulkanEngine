#pragma once
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanSwapChainSipportDetails.h"

namespace Engine
{
    namespace Core
    {
        class VulkanStaticHelper
        {
        public:
            static bool CheckValidationLayerSupport();
            static std::vector<const char *> GetRequiredExtensions();

            static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice &);

            static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &);
            static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

            static uint32_t VkFormatToGLFormat(vk::Format format);
            static vk::Format GLFormatToVkFormat(uint32_t format);

            static bool HasStencilComponent(vk::Format format);

        public:
            static const bool m_bEnableValidationLayers;
            static const std::vector<const char *> m_vValidationLayers;
            static const std::vector<const char *> m_vDeviceExtensions;
        };
    }
}