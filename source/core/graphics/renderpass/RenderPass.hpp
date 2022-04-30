#pragma once

namespace Engine
{
    namespace Resources { class ResourceManager; }
    namespace Core
    {
        class Image;
        namespace Scene {namespace Objects { class RenderObject; }}
        namespace Render
        {
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
                    Builder &addAttachmentDescription(const vk::Format format, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eDontCare,
                                                      vk::AttachmentLoadOp stencilLoadOp = vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp stencilStoreOp = vk::AttachmentStoreOp::eDontCare, vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined, 
                                                      vk::ImageLayout finalLayout = vk::ImageLayout::eColorAttachmentOptimal);

                    Builder &addSubpassDescription(vk::SubpassDescription &&desc);
                    Builder &addSubpassDescription(vk::PipelineBindPoint bindPoint, const std::vector<vk::AttachmentReference> &attachRef, vk::AttachmentReference *depthAttach = nullptr, const std::vector<vk::AttachmentReference> &inputRef = {});

                    Builder &addSubpassDependency(vk::SubpassDependency &&dep);
                    Builder &addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput, 
                    vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlags srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite, 
                    vk::AccessFlags dstAccessMask = vk::AccessFlagBits::eShaderRead, vk::DependencyFlags depFlags = vk::DependencyFlagBits::eByRegion);

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

                void create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root);
                void reCreate();
                void cleanup();

                void begin(vk::CommandBuffer &commandBuffer, std::vector<vk::Framebuffer>& framebuffer);
                void end(vk::CommandBuffer &commandBuffer);

                void render(vk::CommandBuffer& commandBuffer, std::vector<std::shared_ptr<Image>>& images, std::shared_ptr<Scene::Objects::RenderObject> root);

                void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
                void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
                void setRenderArea(vk::Rect2D &&area);

                const std::vector<FInputAttachment>& getAttachments() const { return vAttachments; }

                void pushSubpass(std::shared_ptr<CSubpass>&& subpass);
                const uint32_t getSubpassCount() const { return vSubpasses.size(); }

                operator const vk::RenderPass &() const { return renderPass; }

                vk::RenderPass &get() { return renderPass; }

            private:
                vk::RenderPass createRenderPass();
                std::vector<std::shared_ptr<CSubpass>> vSubpasses;

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