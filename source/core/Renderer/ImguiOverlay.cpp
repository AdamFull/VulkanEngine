#include "ImguiOverlay.h"
#include "WindowHandle.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Materials/MaterialUI.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "VulkanSwapChain.h"

static void check_vk_result(VkResult err) 
{
    if (err == 0) return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0) abort();
}

namespace Engine
{
    void ImguiOverlay::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        fontTexture = std::make_shared<Texture2D>();
        fontMaterial = std::make_shared<MaterialUI>();
        vertexBuffer = std::make_unique<VulkanBuffer>();
        indexBuffer = std::make_unique<VulkanBuffer>();
        m_pUniform = std::make_unique<UniformBuffer<FUniformDataUI>>();

        ImGui::CreateContext();
        BaseInitialize();
        m_pUniform->Create(device, swapchain->GetImages().size());
        CreateResources(device, swapchain);
    }

    void ImguiOverlay::ReCreate(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        fontMaterial->ReCreate();
        m_pUniform->ReCreate(device, swapchain->GetImages().size());
    }

    void ImguiOverlay::Cleanup(std::unique_ptr<Device>& device)
    {
        fontMaterial->Cleanup();
        m_pUniform->Cleanup(device);
    }

    void ImguiOverlay::Destroy(std::unique_ptr<Device>& device)
    {
        ImGui::DestroyContext();
        fontMaterial->Destroy();
        fontTexture->Destroy();
        vertexBuffer->Destroy(device);
        indexBuffer->Destroy(device);
        m_pUniform->Cleanup(device);
    }

    void ImguiOverlay::BaseInitialize()
    {
        // Color scheme
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
		style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		// Dimensions
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(WindowHandle::m_iWidth, WindowHandle::m_iHeight);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    void ImguiOverlay::CreateResources(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        CreateFontResources(device);
    }

    void ImguiOverlay::CreateFontResources(std::unique_ptr<Device>& device)
    {
        ImGuiIO& io = ImGui::GetIO();

		// Create font texture
		unsigned char* fontData;
		int texWidth, texHeight;
		io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

		fontTexture->Load(fontData, texWidth, texHeight, 4, 1, vk::Format::eR8G8B8A8Unorm);
        fontMaterial->Create(fontTexture);
    }

    void ImguiOverlay::NewFrame()
    {
        ImGui::NewFrame();

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        CreateDebugOverlay();
        CreateMenuBar();

        ImGui::Render();
    }

    void ImguiOverlay::Update(std::unique_ptr<Device>& device, float deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImDrawData *drawdata = ImGui::GetDrawData();
        vk::DeviceSize vertexBufferSize = drawdata->TotalVtxCount * sizeof(ImDrawVert);
		vk::DeviceSize indexBufferSize = drawdata->TotalIdxCount * sizeof(ImDrawIdx);

		if ((vertexBufferSize == 0) || (indexBufferSize == 0)) 
        {
			return;
		}

		// Update buffers only if vertex or index count has been changed compared to current buffer size
        auto physProps = device->GetPhysical().getProperties();
        auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
		// Vertex buffer
		if (!vertexBuffer->GetBuffer() || !vertexBuffer->GetMappedMemory()) 
        {
			vertexBuffer->UnmapMem(device);
			vertexBuffer->Destroy(device);
            vertexBuffer->Create(device, sizeof(ImDrawVert), drawdata->TotalVtxCount, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
			vertexBuffer->MapMem(device);
		}

		// Index buffer
		if (!indexBuffer->GetBuffer() || !indexBuffer->GetMappedMemory()) 
        {
			indexBuffer->UnmapMem(device);
			indexBuffer->Destroy(device);
            indexBuffer->Create(device, sizeof(ImDrawIdx), drawdata->TotalIdxCount, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, minOffsetAllignment);
			indexBuffer->MapMem(device);
		}

		// Upload data
		ImDrawVert* vtxDst = (ImDrawVert*)vertexBuffer->GetMappedMemory();
		ImDrawIdx* idxDst = (ImDrawIdx*)indexBuffer->GetMappedMemory();

		for (int n = 0; n < drawdata->CmdListsCount; n++) 
        {
			const ImDrawList* cmd_list = drawdata->CmdLists[n];
			memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmd_list->VtxBuffer.Size;
			idxDst += cmd_list->IdxBuffer.Size;
		}

		// Flush to make writes visible to GPU
		vertexBuffer->Flush(device);
		indexBuffer->Flush(device);

        io.DisplaySize = ImVec2((float)WindowHandle::m_iWidth, (float)WindowHandle::m_iHeight);
        io.DeltaTime = deltaTime;
    }

    void ImguiOverlay::DrawFrame(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer, uint32_t index)
    {
        ImGuiIO& io = ImGui::GetIO();

        fontMaterial->Update(index, m_pUniform->GetUniformBuffer(index));
		fontMaterial->Bind(commandBuffer, index);
        
		vk::Viewport viewport{};
        viewport.width = ImGui::GetIO().DisplaySize.x;
        viewport.height = ImGui::GetIO().DisplaySize.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffer.setViewport(0, 1, &viewport);

        FUniformDataUI ubo{};
        ubo.scale =  glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
        ubo.translate = glm::vec2(-1.0f);
        m_pUniform->UpdateUniformBuffer(device, index, ubo);

		// Render commands
		ImDrawData* imDrawData = ImGui::GetDrawData();
		int32_t vertexOffset = 0;
		int32_t indexOffset = 0;

		if (imDrawData->CmdListsCount > 0) 
        {

			vk::DeviceSize offsets[1] = { 0 };
            commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer->GetBuffer(), offsets);
            commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint16);

			for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[i];
				for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
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

    void ImguiOverlay::ProcessKeys(EActionKey eKey)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (eKey)
        {
            case EActionKey::eMouseLeft:
            {
                io.MouseDown[0] = !io.MouseDown[0];
            }break;
            case EActionKey::eMouseRight:
            {
                io.MouseDown[1] = !io.MouseDown[1];
            }break;
        }
    }

    void ImguiOverlay::ProcessCursor(float fX, float fY)
    {
        ImGuiIO& io = ImGui::GetIO();
        if(io.WantCaptureMouse)
            io.MousePos = ImVec2(fX, fY);
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