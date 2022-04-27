#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CFramebuffer;
            class CSubpass;
            class CRenderPass
            {
            public:
                struct FInputAttachment
                {
                    vk::Format format;
                    vk::ImageUsageFlags usageFlags;
                };

                friend class Builder;
                class Builder
                {
                public:
                    Builder &addAttachmentDescription(vk::AttachmentDescription &&desc);
                    Builder &addAttachmentDescription(const vk::Format format, vk::SampleCountFlagBits samples, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
                                                      vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

                    Builder &addSubpassDescription(vk::SubpassDescription &&desc);
                    Builder &addSubpassDescription(vk::PipelineBindPoint bindPoint, const std::vector<vk::AttachmentReference> &attachRef, vk::AttachmentReference *depthAttach = nullptr, const std::vector<vk::AttachmentReference> &inputRef = {});

                    Builder &addSubpassDependency(vk::SubpassDependency &&dep);
                    Builder &addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::DependencyFlags depFlags);

                    std::unique_ptr<CRenderPass> build();

                private:
                    std::vector<FInputAttachment> vAttachments;
                    std::vector<vk::AttachmentDescription> vAttachDesc;
                    std::vector<vk::SubpassDescription> vSubpassDesc;
                    std::vector<vk::SubpassDependency> vSubpassDep;
                    std::vector<vk::ClearValue> vClearValues;
                };

                CRenderPass() = default;
                CRenderPass(vk::RenderPass &&pass);
                ~CRenderPass();

                void create();
                void reCreate();
                void cleanup();

                void begin(vk::CommandBuffer &commandBuffer);
                void end(vk::CommandBuffer &commandBuffer);

                void render(vk::CommandBuffer& commandBuffer);

                void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
                void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
                void setRenderArea(vk::Rect2D &&area);

                operator const vk::RenderPass &() const { return renderPass; }

                vk::RenderPass &get() { return renderPass; }

            private:
                std::vector<std::shared_ptr<CSubpass>> vSubpasses;
                std::shared_ptr<CFramebuffer> pFramebuffer;

                //ReCreate data
                std::vector<FInputAttachment> vAttachments;
                std::vector<vk::AttachmentDescription> vAttachDesc;
                std::vector<vk::SubpassDescription> vSubpassDesc;
                std::vector<vk::SubpassDependency> vSubpassDep;
                std::vector<vk::ClearValue> vClearValues;

                vk::RenderPass renderPass{nullptr};
                vk::Rect2D renderArea;
            };
        }
    }
}