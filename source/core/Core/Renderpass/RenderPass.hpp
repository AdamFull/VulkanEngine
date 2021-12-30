#pragma once

namespace Core
{
    namespace Render
    {
        class CRenderStage;
        class CRenderPass
        {
        public:
            class CVulkanSubpass : NonCopyable
            {
                CVulkanSubpass(vk::PipelineBindPoint bindPoint, std::vector<vk::AttachmentReference>&& colorAttachments, const std::optional<uint32_t> &depthAttachment) :
                colorAttachments(std::move(colorAttachments))
                {
                    subpassDescription.pipelineBindPoint = bindPoint;
                    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(this->colorAttachments.size());
                    subpassDescription.pColorAttachments = this->colorAttachments.data();

                    if (depthAttachment) 
                    {
                        depthStencilAttachment.attachment = *depthAttachment;
                        depthStencilAttachment.layout = vk::ImageLayout::eDepthAttachmentOptimal;
                        subpassDescription.pDepthStencilAttachment = &depthStencilAttachment;
                    }
                }
                const vk::SubpassDescription &GetSubpassDescription() const { return subpassDescription; }
            private:
                vk::SubpassDescription subpassDescription = {};
		        std::vector<vk::AttachmentReference> colorAttachments;
		        vk::AttachmentReference depthStencilAttachment = {};
            };

            CRenderPass() = default;
            ~CRenderPass();

            void Create(const CRenderStage &renderStage, vk::Format depthFormat, vk::Format surfaceFormat, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1);
        private:
            vk::RenderPass renderPass{nullptr};
        };
    }
}