#pragma once
#include "Resources/ResourceCunstruct.h"
#include "Core/Rendering/RendererBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Image;
        class RenderSystem : public utl::non_copy_movable
        {
        public:
            RenderSystem() = default;
            ~RenderSystem();

            void Create();

            void ReCreate();
            void Cleanup();

            vk::CommandBuffer BeginFrame();
            vk::Result EndFrame();

            std::shared_ptr<Rendering::RendererBase> PushStage(FRendererCreateInfo::ERendererType eType, vk::Extent2D extent);
            void Render(vk::CommandBuffer& commandBuffer);

            /*void BeginRender(vk::CommandBuffer& commandBuffer);
            void EndRender(vk::CommandBuffer& commandBuffer);*/

            /*void BeginPostProcess(vk::CommandBuffer& commandBuffer);
            void EndPostProcess(vk::CommandBuffer& commandBuffer);*/

            // Getters
            inline std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> &GetCommandBuffers() { return m_vCommandBuffers; }
            inline uint32_t GetImageIndex() { return m_iImageIndex; }
            inline bool GetFrameStartFlag() { return m_bFrameStarted; }
            vk::CommandBuffer GetCurrentCommandBuffer() const;
            inline std::shared_ptr<Rendering::RendererBase> GetRenderer(FRendererCreateInfo::ERendererType eType) { return m_pStages->Find(eType); }
            inline std::shared_ptr<Rendering::RendererBase> GetCurrentStage() { return m_pCurrentStage; }

        private:
            void CreateCommandBuffers();
            inline void SetCurrentStage(std::shared_ptr<Rendering::RendererBase> pStage) { m_pCurrentStage = pStage; }

            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> m_vCommandBuffers;
            uint32_t m_iImageIndex{0};
            bool m_bFrameStarted{false};
            
            vk::Extent2D m_ScreenExtent;

            std::shared_ptr<Rendering::RendererBase> m_pStages{nullptr};
            std::shared_ptr<Rendering::RendererBase> m_pCurrentStage{nullptr};
        };
    }
}