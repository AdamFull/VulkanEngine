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
                struct FTextureAttachmentInfo
                {
                    std::string name;
                    vk::Format format;
                    vk::ImageUsageFlags usageFlags;
                };

                CFramebuffer() = default;
                CFramebuffer(std::vector<vk::Framebuffer> &&framebuffers);
                ~CFramebuffer();

                void create(vk::RenderPass& renderPass, vk::Extent2D extent);
                void reCreate(vk::RenderPass& renderPass);
                void cleanup();

                void addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags);

                std::vector<vk::Framebuffer>& get() { return vFramebuffers; }
                std::unordered_map<std::string, std::shared_ptr<Image>>& getImages(uint32_t index) { return mImages[index]; }

            private:
                std::shared_ptr<Image> createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent);
                std::vector<vk::Framebuffer> vFramebuffers;
                std::vector<FTextureAttachmentInfo> attachments;
                vk::Extent2D imagesExtent;
                std::map<uint32_t, std::unordered_map<std::string, std::shared_ptr<Image>>> mImages;
                std::shared_ptr<Image> pDepth;
            };
        }
    }
}