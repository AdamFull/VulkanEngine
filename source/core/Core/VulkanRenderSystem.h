#pragma once
#include "Resources/ResourceCunstruct.h"
#include "Core/Rendering/RendererBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture { class Image; }
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        struct FRenderSystem
        {
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
            uint32_t imageIndex{0};
            bool bFrameStarted{false};
        };

        class RenderSystem
        {
        public:
            RenderSystem() = default;
            ~RenderSystem();

            RenderSystem(const RenderSystem &) = delete;
            void operator=(const RenderSystem &) = delete;
            RenderSystem(RenderSystem &&) = delete;
            RenderSystem &operator=(RenderSystem &&) = delete;

            void Create();

            void ReCreate();
            void Cleanup();

            vk::CommandBuffer BeginFrame();
            vk::Result EndFrame();

            void PushStage(FRendererCreateInfo::ERendererType eType, vk::Extent2D extent);
            void Render(vk::CommandBuffer& commandBuffer);

            /*void BeginRender(vk::CommandBuffer& commandBuffer);
            void EndRender(vk::CommandBuffer& commandBuffer);*/

            /*void BeginPostProcess(vk::CommandBuffer& commandBuffer);
            void EndPostProcess(vk::CommandBuffer& commandBuffer);*/

            // Getters
            inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> &GetCommandBuffers() { return data.vCommandBuffers; }
            inline uint32_t GetImageIndex() { return data.imageIndex; }
            inline bool GetFrameStartFlag() { return data.bFrameStarted; }
            inline vk::CommandBuffer GetCurrentCommandBuffer() const;
            inline std::shared_ptr<Rendering::RendererBase> GetRenderer(FRendererCreateInfo::ERendererType eType) { return m_pStages->Find(eType); }
            inline std::shared_ptr<Rendering::RendererBase> GetCurrentStage() { return m_pCurrentStage; }

        private:
            void CreateCommandBuffers();
            inline void SetCurrentStage(std::shared_ptr<Rendering::RendererBase> pStage) { m_pCurrentStage = pStage; }
            FRenderSystem data;
            
            vk::Extent2D screenExtent;

            std::shared_ptr<Rendering::RendererBase> m_pStages{nullptr};
            std::shared_ptr<Rendering::RendererBase> m_pCurrentStage{nullptr};
        };
    }
}