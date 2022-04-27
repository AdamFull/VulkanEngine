#pragma once

namespace Core
{
    namespace Render
    {
        class CRenderPass
        {
        public:
            class Builder
            {
            public:
                Builder& addAttachmentDescription(vk::AttachmentDescription&& desc);
                Builder& addAttachmentDescription(const vk::Format format, vk::SampleCountFlagBits samples, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
                vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

                Builder& addSubpassDescription(vk::SubpassDescription&& desc);
                Builder& addSubpassDescription(vk::PipelineBindPoint bindPoint, const std::vector<vk::AttachmentReference>& attachRef, vk::AttachmentReference* depthAttach = nullptr, const std::vector<vk::AttachmentReference>& inputRef = {});

                Builder& addSubpassDependency(vk::SubpassDependency&& dep);
                Builder& addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::DependencyFlags depFlags);

                std::unique_ptr<CRenderPass> build();
            private:
                std::vector<vk::AttachmentDescription> vAttachDesc;
                std::vector<vk::SubpassDescription> vSubpassDesc;
                std::vector<vk::SubpassDependency> vSubpassDep;
                std::vector<vk::ClearValue> vClearValues;
            };

            CRenderPass() = default;
            CRenderPass(vk::RenderPass&& pass);

            void begin(vk::CommandBuffer& commandBuffer);
            void end(vk::CommandBuffer& commandBuffer);

            void setRenderArea(uint32_t offset_x, uint32_t offset_y, uint32_t width, uint32_t height);
            void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
            void setRenderArea(vk::Rect2D&& area);

            vk::RenderPass& get() { return renderPass; }
        private:
            vk::RenderPass renderPass{nullptr};
            std::vector<vk::ClearValue> vClearValues;
            vk::Rect2D renderArea;
        };
    }
}