#pragma once
#include "DataTypes/VulkanImage.hpp"
#include "VulkanMainStructures.h"

namespace Engine::SwapChain
{    
    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D ChooseSwapExtent(Main::FVulkanEngine& engine, const vk::SurfaceCapabilitiesKHR& capabilities);
}