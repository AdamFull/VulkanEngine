#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Main
{
    //Window
    void CreateWindowHandle(FVulkanEngine& engine, int width, int height, const char* srWinName);

    //Device
    void CreateInstance(FVulkanEngine& engine);
    void SetupDebugMessenger(FVulkanEngine& engine);
    void CreateSurface(FVulkanEngine& engine);
    void CreateLogicalDevice(FVulkanEngine& engine);
    void CreateCommandPool(FVulkanEngine& engine);

    //Pipeline
    void CreateVulkanPipeline(FVulkanEngine& engine, Pipeline::FPipelineConfigInfo& configInfo);

    //SwapChain
    void CreateSwapChain(FVulkanEngine& engine);
    void CreateSwapChainImageViews(FVulkanEngine& engine);
    void CreateDepthResources(FVulkanEngine& engine);
    void CreateMSAAResources(FVulkanEngine& engine);
    void CreateRenderPass(FVulkanEngine& engine);
    void CreateFramebuffers(FVulkanEngine& engine);
    void CreateSyncObjects(FVulkanEngine& engine);
}