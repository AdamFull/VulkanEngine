#pragma once
#include "VulkanMainStructures.h"

namespace Engine::Main
{
    //Window
    void CreateWindowHandle(FVulkanRenderInstance& renderInstance, int width, int height, const char* srWinName);

    //Device
    void CreateInstance(FVulkanRenderInstance& renderInstance);
    void SetupDebugMessenger(FVulkanRenderInstance& renderInstance);
    void CreateSurface(FVulkanRenderInstance& renderInstance);
    void CreateLogicalDevice(FVulkanRenderInstance& renderInstance);
    void CreateCommandPool(FVulkanRenderInstance& renderInstance);

    //Pipeline
    void CreateVulkanPipeline(FVulkanRenderInstance& renderInstance, Pipeline::FPipelineConfigInfo& configInfo);

    //SwapChain
    void CreateSwapChain(FVulkanRenderInstance& renderInstance);
    void CreateSwapChainImageViews(FVulkanRenderInstance& renderInstance);
    void CreateDepthResources(FVulkanRenderInstance& renderInstance);
    void CreateRenderPass(FVulkanRenderInstance& renderInstance);
    void CreateFramebuffers(FVulkanRenderInstance& renderInstance);
    void CreateSyncObjects(FVulkanRenderInstance& renderInstance);
}