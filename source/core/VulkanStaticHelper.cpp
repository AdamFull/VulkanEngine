#include "Vulkan/VulkanStaticHelper.h"

namespace Engine
{
#ifdef NDEBUG
    const bool VulkanStaticHelper::m_bEnableValidationLayers = false;
#else
    const bool VulkanStaticHelper::m_bEnableValidationLayers = true;
#endif

    const std::vector<const char *> VulkanStaticHelper::m_vValidationLayers =
        {
            "VK_LAYER_OW_OVERLAY",
            "VK_LAYER_KHRONOS_validation"
        };

    const std::vector<const char *> VulkanStaticHelper::m_vDeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool VulkanStaticHelper::CheckValidationLayerSupport()
    {
        auto availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char *layerName : m_vValidationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                std::cout << layerProperties.layerName << std::endl;
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    std::vector<const char *> VulkanStaticHelper::GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (m_bEnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanStaticHelper::CheckDeviceExtensionSupport(const vk::PhysicalDevice &device)
    {
        std::set<std::string> sRequiredExtensions(m_vDeviceExtensions.begin(), m_vDeviceExtensions.end());

        for (const auto &extension : device.enumerateDeviceExtensionProperties())
        {
            sRequiredExtensions.erase(extension.extensionName);
        }

        return sRequiredExtensions.empty();
    }

    vk::SurfaceFormatKHR VulkanStaticHelper::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        {
            return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
        }

        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanStaticHelper::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
    {
        vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablePresentMode;
            }
            else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
            {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }

    std::vector<char> VulkanStaticHelper::ReadFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    bool VulkanStaticHelper::HasStencilComponent(vk::Format format)
    {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }
}