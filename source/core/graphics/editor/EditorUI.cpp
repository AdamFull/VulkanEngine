#include "EditorUI.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "graphics/window/WinCallbacks.h"
#include "graphics/window/WindowHandle.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/buffer/VulkanBuffer.h"
#include "graphics/buffer/UniformHandler.hpp"
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/VulkanInitializers.h"

#include "windows/DebugWindow.h"
#include "windows/ConsoleWindow.h"
#include "windows/LogWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/ViewportWindow.h"

#include "graphics/scene/objects/RenderObject.h"

#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;

template<>
std::unique_ptr<CEditorUI> utl::singleton<CEditorUI>::_instance{nullptr};

CEditorUI::~CEditorUI()
{
    cleanup();
    ImGui::DestroyContext();
}

void CEditorUI::create(vk::RenderPass& renderPass, uint32_t subpass)
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
    vk::Result res = CDevice::inst()->create(pool_info, &descriptorPool);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor pool.");

    ImGui::CreateContext();
    baseInitialize();

    vWindows.emplace_back(std::make_shared<Editor::CDebugWindow>());
    vWindows.emplace_back(std::make_shared<Editor::CHierarchyWindow>());
    vWindows.emplace_back(std::make_shared<Editor::CInspectorWindow>());
    vWindows.emplace_back(std::make_shared<Editor::CViewportWindow>());

    ImGui_ImplGlfw_InitForVulkan(CWindowHandle::inst()->getWindowInstance(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = CDevice::inst()->getVkInstance();
    init_info.PhysicalDevice = CDevice::inst()->getPhysical();
    init_info.Device = CDevice::inst()->getLogical();
    init_info.QueueFamily = CDevice::inst()->findQueueFamilies().graphicsFamily.value();
    init_info.Queue = CDevice::inst()->getGraphicsQueue();

    // pipeline cache is a potential future optimization, ignoring for now
    init_info.PipelineCache = CDevice::inst()->getPipelineCache();
    init_info.DescriptorPool = descriptorPool;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = 2;
    init_info.ImageCount = CDevice::inst()->getFramesInFlight();
    init_info.MSAASamples = vk::SampleCountFlagBits::e1;
    init_info.Subpass = subpass;
    //init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderPass);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    cmdBuf.submitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    for (auto &overlay : vWindows)
    {
        overlay->create();
    }
}

void CEditorUI::reCreate()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    auto& renderPass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->get();
    auto subpass = CRenderSystem::inst()->getCurrentStage()->getRenderPass()->getCurrentSubpass();

    ImGui_ImplGlfw_InitForVulkan(CWindowHandle::inst()->getWindowInstance(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = CDevice::inst()->getVkInstance();
    init_info.PhysicalDevice = CDevice::inst()->getPhysical();
    init_info.Device = CDevice::inst()->getLogical();
    init_info.QueueFamily = CDevice::inst()->findQueueFamilies().graphicsFamily.value();
    init_info.Queue = CDevice::inst()->getGraphicsQueue();

    // pipeline cache is a potential future optimization, ignoring for now
    init_info.PipelineCache = CDevice::inst()->getPipelineCache();
    init_info.DescriptorPool = descriptorPool;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = 2;
    init_info.ImageCount = CDevice::inst()->getFramesInFlight();
    init_info.MSAASamples = vk::SampleCountFlagBits::e1;
    init_info.Subpass = subpass;
    //init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderPass);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    cmdBuf.submitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    for (auto &overlay : vWindows)
    {
        overlay->reCreate();
    }
}

void CEditorUI::cleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    CDevice::inst()->destroy(&descriptorPool);
}

void CEditorUI::baseInitialize()
{
    // Color scheme
    /*ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.4f, 0.0f, 0.1f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.4f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.4f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.4f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.4f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.4f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);*/
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2(CWindowHandle::m_iWidth, CWindowHandle::m_iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void CEditorUI::newFrame()
{
    if (bEnabled)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();

        //ImGui::ShowDemoWindow();
        for (auto &overlay : vWindows)
        {
            overlay->draw();
        }
    }
}

void CEditorUI::drawFrame(vk::CommandBuffer commandBuffer, uint32_t index)
{
    if (bEnabled)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Render();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
    }
}

void CEditorUI::onFocusChange(int focused)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void CEditorUI::onCursorEnter(int enter)
{
}

void CEditorUI::onMouseButtonDown(int button, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[button] = (action == 1);
}

void CEditorUI::onMousePositionUpdate(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        io.MousePos = ImVec2(xpos, ypos);
    }
}

void CEditorUI::onMouseScroll(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += xpos;
    io.MouseWheel += ypos;
}

void CEditorUI::onKeyboardInput(int key, int scancode, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();
    if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
    {
        if (action == 1)
        {
            io.KeysDown[key] = true;
        }
        if (action == 0)
        {
            io.KeysDown[key] = false;
        }
    }

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void CEditorUI::onInputChar(unsigned int c)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

void CEditorUI::onMonitorEvent(int monitor)
{
}