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

        WindowHandle::FocusCallback.attach(this, &ImguiOverlay::UpdateFocusStatus);
        WindowHandle::CharInputCallback.attach(this, &ImguiOverlay::UpdateInputChar);
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
        io.ConfigFlags |= 1 << 6;
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
        UpdateControls(deltaTime);
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
    }

    void ImguiOverlay::UpdateControls(float fDeltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)WindowHandle::m_iWidth, (float)WindowHandle::m_iHeight);
		io.DeltaTime = fDeltaTime;

		io.MousePos = ImVec2(controls.fMouseX, controls.fMouseY);
		io.MouseDown[0] = controls.bLMbtn;
		io.MouseDown[1] = controls.bRMbtn;

        controls.bLMbtn = true;
        controls.bRMbtn = true;
    }

    void ImguiOverlay::UpdateFocusStatus(int focus)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(focus != 0);
    }

    void ImguiOverlay::UpdateInputChar(unsigned char c)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
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
        switch (eKey)
        {
            case EActionKey::eMouseLeft:
            {
                controls.bLMbtn = false;
            }break;
            case EActionKey::eMouseRight:
            {
                controls.bRMbtn = false;
            }break;
        }
    }

    void ImguiOverlay::ProcessCursor(float fX, float fY)
    {
        ImGuiIO& io = ImGui::GetIO();
        if(io.WantCaptureMouse)
        {
            controls.fMouseX = fX;
            controls.fMouseY = fY;
        }
    }

    void ImguiOverlay::CreateDebugOverlay()
    {
        float fFrameTime = 1000.0f / ImGui::GetIO().Framerate;
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();
        auto pos = camera->GetPosition();
        float position[3] = {pos.x, pos.y, pos.z};
        auto rot = camera->GetRotation();
        float rotation[3] = {rot.x, rot.y, rot.z};

        int viewport_size[2] = {WindowHandle::m_iWidth, WindowHandle::m_iHeight};
        
        std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
        frameTimes.back() = fFrameTime;

        ImGui::Begin("Debug info");

        ImGui::Text("Frame info");
        char overlay[32];
        sprintf(overlay, "dt: %.3f | fps: %.3f", fFrameTime, ImGui::GetIO().Framerate);
        ImGui::PlotLines("", &frameTimes[0], 50, 0, overlay, frameTimeMin, frameTimeMax, ImVec2(0, 80));

        ImGui::InputInt2("Viewport size", viewport_size);

        ImGui::Text("Camera");
		ImGui::InputFloat3("Position", (float*)position);
        ImGui::InputFloat3("Rotation", (float*)rotation);

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