#pragma once

namespace Engine
{
    namespace Core
    {
        class Image;
        namespace Render
        {
            class CFramebuffer
            {
            public:
                friend class Builder;
                class Builder
                {
                public:
                    struct FAttachmentInfo
                    {
                        vk::Format format;
                        vk::ImageUsageFlags usageFlags;
                    };

                    Builder &addImage(vk::Format format, vk::ImageUsageFlags usageFlags);
                    std::unique_ptr<CFramebuffer> build(vk::RenderPass &renderPass, vk::Extent2D extent);

                private:
                    std::shared_ptr<Image> createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent);
                    std::vector<FAttachmentInfo> attachments;
                };

                CFramebuffer() = default;
                CFramebuffer(std::vector<vk::Framebuffer> &&framebuffers);
                ~CFramebuffer();

                void reCreate(vk::RenderPass& renderPass);
                void cleanup();

            private:
                std::vector<vk::Framebuffer> vFramebuffers;
                std::map<uint32_t, std::vector<std::shared_ptr<Image>>> mImages;
                std::shared_ptr<Image> pDepth;
            };
        }
    }
}