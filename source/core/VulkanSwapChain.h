#pragma once
#include "DataTypes/Special/VulkanImage.hpp"
#include "VulkanMainStructures.h"

namespace Engine::SwapChain
{    
    vk::Result AcquireNextImage(Main::FVulkanEngine& engine, uint32_t *imageIndex);
    vk::Result SubmitCommandBuffers(Main::FVulkanEngine& engine, const vk::CommandBuffer *buffers, uint32_t *imageIndex);

    void Bind(Main::FVulkanEngine& engine, vk::CommandBuffer commandBuffer);

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D ChooseSwapExtent(Main::FVulkanEngine& engine, const vk::SurfaceCapabilitiesKHR& capabilities);
}