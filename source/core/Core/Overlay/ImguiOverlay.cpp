#include "ImguiOverlay.h"
#include "backends/imgui_impl_glfw.h"

#include "Core/Window/WinCallbacks.h"
#include "Core/Window/WindowHandle.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanBuffer.h"
#include "Resources/ResourceManager.h"
#include "Resources/Textures/Image.h"
#include "Resources/Materials/MaterialUI.h"
#include "Core/VulkanSwapChain.h"
#include "Core/VulkanInitializers.h"

#include "Overlays/OverlayDebug.h"
#include "Overlays/OverlayConsole.h"
#include "Overlays/OverlayLog.h"
#include "Overlays/OverlayPropertyEditor.h"
#include "Overlays/OverlaySceneGraph.h"

#include "Core/Scene/Objects/RenderObject.h"

#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

ImguiOverlay::~ImguiOverlay()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //fontMaterial->Destroy();
    m_pUniform->Cleanup();
}

void ImguiOverlay::Create(std::shared_ptr<ResourceManager> pResMgr, std::shared_ptr<Scene::Objects::RenderObject> pRoot)
{
    fontTexture = std::make_shared<Image>();
    fontMaterial = std::make_shared<MaterialUI>();
    vertexBuffer = std::make_shared<VulkanBuffer>();
    indexBuffer = std::make_shared<VulkanBuffer>();
    m_pUniform = std::make_shared<UniformBuffer>();

    ImGui::CreateContext();
    BaseInitialize();
    m_pUniform->Create(USwapChain->GetFramesInFlight(), sizeof(FUniformDataUI));
    CreateFontResources(pResMgr);

    m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayDebug>("Debug info"));
    //m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayConsole>("Console"));
    //m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayLog>("Log"));
    m_vOverlays.emplace_back(std::make_shared<Overlay::OverlaySceneGraph>("Scene", pRoot));
    m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayPropertyEditor>("Property editor"));

    ImGui_ImplGlfw_InitForVulkan(UWinHandle->GetWindowInstance(), true);
}

void ImguiOverlay::ReCreate()
{
    fontMaterial->ReCreate();
    m_pUniform->ReCreate(USwapChain->GetFramesInFlight());
}

void ImguiOverlay::Cleanup()
{
    fontMaterial->Cleanup();
    m_pUniform->Cleanup();
}

void ImguiOverlay::BaseInitialize()
{
    // Color scheme
    ImGuiStyle &style = ImGui::GetStyle();
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
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void ImguiOverlay::CreateFontResources(std::shared_ptr<ResourceManager> pResMgr)
{
    ImGuiIO &io = ImGui::GetIO();

    // Create font texture
    unsigned char *fontData;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

    vk::Format format;
    ktxTexture *texture;
    bool result = Loaders::ImageLoader::AllocateRawDataAsKTXTexture(fontData, &texture, &format, texWidth, texHeight, 1, 2);

    fontTexture->InitializeTexture(texture, format);
    fontTexture->LoadFromMemory(texture, format);
    fontMaterial->AddTexture(ETextureAttachmentType::eDiffuseAlbedo, fontTexture);
    fontMaterial->Create(pResMgr);

    Loaders::ImageLoader::Close(&texture);
}

void ImguiOverlay::NewFrame()
{
    if (bEnabled)
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::DockSpaceOverViewport();

        // ImGui::ShowDemoWindow();
        for (auto &overlay : m_vOverlays)
        {
            overlay->Draw();
        }

        ImGui::Render();
    }
}

void ImguiOverlay::Update(float deltaTime)
{
    if (bEnabled)
    {
        ImDrawData *drawdata = ImGui::GetDrawData();
        vk::DeviceSize vertexBufferSize = drawdata->TotalVtxCount * sizeof(ImDrawVert);
        vk::DeviceSize indexBufferSize = drawdata->TotalIdxCount * sizeof(ImDrawIdx);

        if ((vertexBufferSize == 0) || (indexBufferSize == 0))
        {
            return;
        }

        // Update buffers only if vertex or index count has been changed compared to current buffer size
        auto physProps = UDevice->GetPhysical().getProperties();
        auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
        // Vertex buffer
        if (!vertexBuffer->GetBuffer() /*|| vertexCount != drawdata->TotalVtxCount*/)
        {
            vertexBuffer->UnmapMem();
            vertexBuffer->Clean();
            vertexBuffer->Create(sizeof(ImDrawVert), 10000, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
            vertexBuffer->MapMem();
            vertexCount = drawdata->TotalVtxCount;
        }

        // Index buffer
        if (!indexBuffer->GetBuffer() /*|| indexCount != drawdata->TotalIdxCount*/)
        {
            indexBuffer->UnmapMem();
            indexBuffer->Clean();
            indexBuffer->Create(sizeof(ImDrawIdx), 20000, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
            indexBuffer->MapMem();
            indexCount = drawdata->TotalIdxCount;
        }

        // Upload data
        ImDrawVert *vtxDst = (ImDrawVert *)vertexBuffer->GetMappedMemory();
        ImDrawIdx *idxDst = (ImDrawIdx *)indexBuffer->GetMappedMemory();

        for (int n = 0; n < drawdata->CmdListsCount; n++)
        {
            const ImDrawList *cmd_list = drawdata->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Flush to make writes visible to GPU
        vertexBuffer->Flush();
        indexBuffer->Flush();
    }
}

void ImguiOverlay::DrawFrame(vk::CommandBuffer commandBuffer, uint32_t index)
{
    if (bEnabled)
    {
        ImGuiIO &io = ImGui::GetIO();

        auto& buffer = m_pUniform->GetUniformBuffer(index);
        fontMaterial->Update(buffer->GetDscriptor(), index);
        fontMaterial->Bind(commandBuffer, index);

        vk::Viewport viewport = Initializers::Viewport(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        commandBuffer.setViewport(0, 1, &viewport);

        FUniformDataUI ubo{};
        ubo.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
        ubo.translate = glm::vec2(-1.0f);
        m_pUniform->UpdateUniformBuffer(index, &ubo);

        // Render commands
        ImDrawData *imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        if (imDrawData->CmdListsCount > 0)
        {

            vk::DeviceSize offsets[1] = {0};
            commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer->GetBuffer(), offsets);
            commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint16);

            for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
            {
                const ImDrawList *cmd_list = imDrawData->CmdLists[i];
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
                {
                    const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
                    vk::Rect2D scissorRect;
                    scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                    scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                    scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                    scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                    commandBuffer.setScissor(0, 1, &scissorRect);
                    commandBuffer.drawIndexed(pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                    indexOffset += pcmd->ElemCount;
                }
                vertexOffset += cmd_list->VtxBuffer.Size;
            }
        }
    }
}

std::unique_ptr<VulkanBuffer> &ImguiOverlay::GetBuffer(uint32_t index)
{
    return m_pUniform->GetUniformBuffer(index);
}

void ImguiOverlay::OnFocusChange(int focused)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void ImguiOverlay::OnCursorEnter(int enter)
{
}

void ImguiOverlay::OnMouseButtonDown(int button, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[button] = (action == 1);
}

void ImguiOverlay::OnMousePositionUpdate(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        io.MousePos = ImVec2(xpos, ypos);
    }
}

void ImguiOverlay::OnMouseScroll(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += xpos;
    io.MouseWheel += ypos;
}

void ImguiOverlay::OnKeyboardInput(int key, int scancode, int action, int mods)
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

void ImguiOverlay::OnInputChar(unsigned int c)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

void ImguiOverlay::OnMonitorEvent(int monitor)
{
}