#include "ImguiOverlay.h"
#include "WindowHandle.h"
#include "VulkanDevice.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "VulkanSwapChain.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

static void check_vk_result(VkResult err) 
{
    if (err == 0) return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0) abort();
}

namespace Engine
{
    void ImguiOverlay::Create(std::unique_ptr<WindowHandle>& winhandle, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        vk::DescriptorPoolSize pool_sizes[] =
        {
            {vk::DescriptorType::eSampler, 1000},
            {vk::DescriptorType::eCombinedImageSampler, 1000},
            {vk::DescriptorType::eSampledImage, 1000},
            {vk::DescriptorType::eStorageImage, 1000},
            {vk::DescriptorType::eUniformTexelBuffer, 1000},
            {vk::DescriptorType::eStorageTexelBuffer, 1000},
            {vk::DescriptorType::eUniformBuffer, 1000},
            {vk::DescriptorType::eStorageBuffer, 1000},
            {vk::DescriptorType::eUniformBufferDynamic, 1000},
            {vk::DescriptorType::eStorageBufferDynamic, 1000},
            {vk::DescriptorType::eInputAttachment, 1000}
        };

        vk::DescriptorPoolCreateInfo pool_info = {};
        pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        m_descriptorPool = device->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(pool_info);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(winhandle->GetWindowInstance(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device->GetVkInstance().get();
        init_info.PhysicalDevice = device->GetPhysical();
        init_info.Device = device->GetLogical().get();
        init_info.QueueFamily = device->FindQueueFamilies().graphicsFamily.value();
        init_info.Queue = device->GetGraphicsQueue();
        init_info.MSAASamples = (VkSampleCountFlagBits)device->GetSamples();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_descriptorPool;

        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = 2;
        init_info.ImageCount = swapchain->GetImages().size();
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, swapchain->GetRenderPass());

        auto commandBuffer = device->BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        device->EndSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void ImguiOverlay::Destroy(std::unique_ptr<Device>& device)
    {
        device->Destroy(m_descriptorPool);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiOverlay::StartFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiOverlay::Render(vk::CommandBuffer commandBuffer)
    {
        ImGui::Render();
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
    }

    void ImguiOverlay::ProcessInterface()
    {
        //if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        CreateDebugOverlay();
        CreateMenuBar();
    }

    void ImguiOverlay::CreateDebugOverlay()
    {
        float fFrameTime = 1000.0f / ImGui::GetIO().Framerate;
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        auto pos = camera->GetPosition();
        /*if (refresh_time == 0.0)
            refresh_time = ImGui::GetTime();
        while (refresh_time < ImGui::GetTime())
        {
            values[values_offset] = fFrameTime;
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            refresh_time += 1.0f / 60.0f;
        }*/

        ImGui::Begin("Debug info");
        //ImGui::Checkbox("Demo Window", &show_demo_window);

        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        //ImGui::ColorEdit3("clear color", (float *)&clear_color);

        //ImGui::SameLine();

        /*char overlay[32];
        sprintf(overlay, "Frame time %f", fFrameTime);
        ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 2.0f, ImVec2(0, 80.0f));*/
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", fFrameTime, ImGui::GetIO().Framerate);
        ImGui::Text("Camera position: {x:%.1f, y:%.1f, z:%.1f}", pos.x, pos.y, pos.z);

        ImGui::End();
    }

    void ImguiOverlay::CreateMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ShowMenuFile();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void ImguiOverlay::ShowMenuFile()
    {
        ImGui::MenuItem("AppMenu", NULL, false, false);
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {}
        if (ImGui::MenuItem("Exit")) {}
    }
}