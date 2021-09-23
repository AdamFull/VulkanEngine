#pragma once
#include "DataTypes/Special/VulkanImage.hpp"
#include "DataTypes/Core/WindowHandleStruct.h"
#include "DataTypes/Core/VulkanDeviceStruct.h"
#include "DataTypes/Core/VulkanSwapChainStruct.h"
#include "DataTypes/Core/VulkanPipelineStruct.h"
#include "DataTypes/Special/VulkanTexture.h"

namespace Engine::Main
{
    struct FVulkanEngine
    {
        //Window
        Window::FWindowHandle window;
        //Device
        Device::FVulkanDevice device;
        //Pipeline
        Pipeline::FVulkanPipeline pipeline;
        //SwapChain
        SwapChain::FVulkanSwapChain swapchain;

        //Texture. Temporary here
        VulkanTexture texture;
    };


}