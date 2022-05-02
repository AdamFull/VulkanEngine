#include "ImguiOverlay.h"
#include "imgui_impl_glfw.h"

#include "graphics/window/WinCallbacks.h"
#include "graphics/window/WindowHandle.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/buffer/VulkanBuffer.h"
#include "graphics/buffer/UniformHandler.hpp"
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "resources/materials/MaterialUI.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanInitializers.h"

#include "Overlays/OverlayDebug.h"
#include "Overlays/OverlayConsole.h"
#include "Overlays/OverlayLog.h"
#include "Overlays/OverlayPropertyEditor.h"
#include "Overlays/OverlaySceneGraph.h"

#include "graphics/scene/objects/RenderObject.h"

#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Window;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;

CImguiOverlay::~CImguiOverlay()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //fontMaterial->Destroy();
    m_pUniformHandle->cleanup();
}

void CImguiOverlay::create(std::shared_ptr<Scene::CRenderObject> pRoot, vk::RenderPass& renderPass, uint32_t subpass)
{
    fontTexture = std::make_shared<CImage>();
    fontMaterial = std::make_shared<CMaterialUI>();
    vertexBuffer = std::make_shared<CVulkanBuffer>();
    indexBuffer = std::make_shared<CVulkanBuffer>();
    m_pUniformHandle = std::make_shared<CUniformHandler>();

    ImGui::CreateContext();
    baseInitialize();
    createFontResources(renderPass, subpass);

    auto uniformBlock = fontMaterial->getPipeline()->getShader()->getUniformBlock("FUniformDataUI");
    if(uniformBlock)
        m_pUniformHandle->create(uniformBlock.value());
    else
        throw std::runtime_error("Cannot create uniform block!");

    m_vOverlays.emplace_back(std::make_shared<Overlay::COverlayDebug>("Debug info"));
    //m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayConsole>("Console"));
    //m_vOverlays.emplace_back(std::make_shared<Overlay::OverlayLog>("Log"));
    m_vOverlays.emplace_back(std::make_shared<Overlay::COverlaySceneGraph>("Scene", pRoot));
    m_vOverlays.emplace_back(std::make_shared<Overlay::COverlayPropertyEditor>("Property editor"));

    ImGui_ImplGlfw_InitForVulkan(UWinHandle->getWindowInstance(), true);
}

void CImguiOverlay::reCreate()
{
    fontMaterial->reCreate();
    m_pUniformHandle->reCreate();
}

void CImguiOverlay::cleanup()
{
    fontMaterial->cleanup();
    m_pUniformHandle->cleanup();
}

void CImguiOverlay::baseInitialize()
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
    io.DisplaySize = ImVec2(CWindowHandle::m_iWidth, CWindowHandle::m_iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void CImguiOverlay::createFontResources(vk::RenderPass& renderPass, uint32_t subpass)
{
    ImGuiIO &io = ImGui::GetIO();

    // Create font texture
    unsigned char *fontData;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

    vk::Format format;
    ktxTexture *texture;
    bool result = Loaders::CImageLoader::allocateRawDataAsKTXTexture(fontData, &texture, &format, texWidth, texHeight, 1, 2);

    fontTexture->initializeTexture(texture, format);
    fontTexture->loadFromMemory(texture, format);
    fontMaterial->addTexture("fontSampler", fontTexture);
    fontMaterial->create(renderPass, subpass);

    Loaders::CImageLoader::close(&texture);
}

void CImguiOverlay::newFrame()
{
    if (bEnabled)
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::DockSpaceOverViewport();

        //ImGui::ShowDemoWindow();
        for (auto &overlay : m_vOverlays)
        {
            overlay->draw();
        }

        ImGui::Render();
    }
}

void CImguiOverlay::update(float deltaTime)
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
        auto physProps = UDevice->getPhysical().getProperties();
        auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
        // Vertex buffer
        if (!vertexBuffer->getBuffer() /*|| vertexCount != drawdata->TotalVtxCount*/)
        {
            vertexBuffer->unmapMem();
            vertexBuffer->clean();
            vertexBuffer->create(sizeof(ImDrawVert), 10000, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
            vertexBuffer->mapMem();
            vertexCount = drawdata->TotalVtxCount;
        }

        // Index buffer
        if (!indexBuffer->getBuffer() /*|| indexCount != drawdata->TotalIdxCount*/)
        {
            indexBuffer->unmapMem();
            indexBuffer->clean();
            indexBuffer->create(sizeof(ImDrawIdx), 20000, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
            indexBuffer->mapMem();
            indexCount = drawdata->TotalIdxCount;
        }

        // Upload data
        ImDrawVert *vtxDst = (ImDrawVert *)vertexBuffer->getMappedMemory();
        ImDrawIdx *idxDst = (ImDrawIdx *)indexBuffer->getMappedMemory();

        for (int n = 0; n < drawdata->CmdListsCount; n++)
        {
            const ImDrawList *cmd_list = drawdata->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Flush to make writes visible to GPU
        vertexBuffer->flush();
        indexBuffer->flush();
    }
}

void CImguiOverlay::drawFrame(vk::CommandBuffer commandBuffer, uint32_t index)
{
    if (bEnabled)
    {
        ImGuiIO &io = ImGui::GetIO();

        auto& buffer = m_pUniformHandle->getUniformBuffer(index);
        auto pipeline = fontMaterial->getPipeline();
        auto descriptor = buffer->getDscriptor();
        fontMaterial->update(descriptor, index);
        fontMaterial->bind(commandBuffer, index);

        vk::Viewport viewport = Initializers::Viewport(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        commandBuffer.setViewport(0, 1, &viewport);

        m_pUniformHandle->set("scale", glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y), index);
        m_pUniformHandle->set("translate", glm::vec2(-1.0f), index);
        m_pUniformHandle->flush(commandBuffer, pipeline);

        // Render commands
        ImDrawData *imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        if (imDrawData->CmdListsCount > 0)
        {

            vk::DeviceSize offsets[1] = {0};
            auto buffer = vertexBuffer->getBuffer();
            commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
            commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint16);

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

std::unique_ptr<CVulkanBuffer> &CImguiOverlay::getBuffer(uint32_t index)
{
    return m_pUniformHandle->getUniformBuffer(index);
}

void CImguiOverlay::onFocusChange(int focused)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void CImguiOverlay::onCursorEnter(int enter)
{
}

void CImguiOverlay::onMouseButtonDown(int button, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[button] = (action == 1);
}

void CImguiOverlay::onMousePositionUpdate(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        io.MousePos = ImVec2(xpos, ypos);
    }
}

void CImguiOverlay::onMouseScroll(float xpos, float ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += xpos;
    io.MouseWheel += ypos;
}

void CImguiOverlay::onKeyboardInput(int key, int scancode, int action, int mods)
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

void CImguiOverlay::onInputChar(unsigned int c)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

void CImguiOverlay::onMonitorEvent(int monitor)
{
}