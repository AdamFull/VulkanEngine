#pragma once

namespace Engine
{
    namespace Core
    {
        class CImage;
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
                void reCreate(vk::RenderPass& renderPass, vk::Extent2D extent);
                void cleanup();

                void addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags);

                std::vector<vk::Framebuffer>& get() { return vFramebuffers; }
                vk::Framebuffer& getFramebuffer(uint32_t index) { return vFramebuffers[index]; }
                vk::Framebuffer& getCurrentFramebuffer();
                std::unordered_map<std::string, std::shared_ptr<CImage>>& getImages(uint32_t index) { return mImages[index]; }
                std::unordered_map<std::string, std::shared_ptr<CImage>>& getCurrentImages();
                std::shared_ptr<CImage>& getDepthImage() { return vDepth.front(); }
                static std::shared_ptr<CImage> createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent);
            private:
                std::vector<vk::Framebuffer> vFramebuffers;
                std::vector<FTextureAttachmentInfo> attachments;
                vk::Extent2D imagesExtent;
                std::map<uint32_t, std::unordered_map<std::string, std::shared_ptr<CImage>>> mImages;
                std::vector<std::shared_ptr<CImage>> vDepth;
                bool bIsClean{false};
            };
        }
    }
}