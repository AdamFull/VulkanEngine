#pragma once
#include "DataTypes/VulkanImage.hpp"
#include "DataTypes/WindowHandleStruct.h"
#include "DataTypes/VulkanDeviceStruct.h"
#include "DataTypes/VulkanSwapChainStruct.h"
#include "DataTypes/VulkanPipelineStruct.h"

namespace Engine::Main
{
    struct FVulkanRenderInstance
    {
        //Window
        Window::FWindowHandle window;
        //Device
        Device::FVulkanDevice device;
        //Pipeline
        Pipeline::FVulkanPipeline pipeline;
        //SwapChain
        SwapChain::FVulkanSwapChain swapchain;
    };


}