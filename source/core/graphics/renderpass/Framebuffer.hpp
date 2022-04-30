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
                    std::unique_ptr<CFramebuffer> build(vk::RenderPass &renderPass);

                private:
                    std::vector<FAttachmentInfo> attachments;
                };

                CFramebuffer() = default;
                CFramebuffer(std::vector<vk::Framebuffer> &&framebuffers);
                ~CFramebuffer();

                void create(vk::RenderPass& renderPass, vk::Extent2D extent);
                void reCreate(vk::RenderPass& renderPass);
                void cleanup();

                std::vector<vk::Framebuffer>& get() { return vFramebuffers; }
                std::vector<std::shared_ptr<Image>>& getImages(uint32_t index) { return mImages[index]; }

            private:
                std::shared_ptr<Image> createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent);
                std::vector<vk::Framebuffer> vFramebuffers;
                std::vector<Builder::FAttachmentInfo> attachments;
                vk::Extent2D imagesExtent;
                std::map<uint32_t, std::vector<std::shared_ptr<Image>>> mImages;
                std::shared_ptr<Image> pDepth;
            };
        }
    }
}