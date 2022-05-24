#pragma once

namespace Engine
{
    namespace Core
    {
        class CImage;
        namespace Render
        {
            struct FFramebufferAttachmentInfo
            {
                std::string name;
                vk::Format format;
                vk::ImageUsageFlags usageFlags;
            };

            class CSubpass;
            class CFramebufferNew
            {
            public:
                //Common part
                void create();
                void reCreate();
                void cleanup();

                //Renderpass part
                void begin(vk::CommandBuffer &commandBuffer);
                void end(vk::CommandBuffer &commandBuffer);
                void render(vk::CommandBuffer& commandBuffer);

                void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
                void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
                void setRenderArea(vk::Rect2D &&area);

                void setFlipViewport(vk::Bool32 value) { flipViewport = value; }

                void pushSubpass(std::shared_ptr<CSubpass>&& subpass);
                const uint32_t getSubpassCount() const { return vSubpasses.size(); }
                vk::SubpassDescription& getCurrentDescription() { return vSubpassDesc.at(currentSubpassIndex); }
                uint32_t getCurrentSubpass() { return currentSubpassIndex; }

                //Framebuffer part
                void addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags);

                vk::Framebuffer& getFramebuffer(uint32_t index) { return vFramebuffers[index]; }
                vk::Framebuffer& getCurrentFramebuffer();
            private:
                void createRenderPass();
                void createFramebuffer();
                static std::shared_ptr<CImage> createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent);

                uint32_t getCurrentFrameProxy();
                static bool isColorAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eColorAttachment); }
                static bool isDepthAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment); }
                static bool isSampled(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eSampled); }
                static bool isInputAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eInputAttachment); }
            private:
                //Common part
                bool bIsClean{false};
                vk::Rect2D renderArea;

                //Renderpass part
                vk::RenderPass renderPass{nullptr};
                std::vector<std::shared_ptr<CSubpass>> vSubpasses;
                vk::Bool32 flipViewport{VK_FALSE};
                uint32_t currentSubpassIndex{0};

                std::vector<vk::AttachmentDescription> vAttachDesc;
                std::vector<vk::SubpassDescription> vSubpassDesc;
                std::vector<vk::SubpassDependency> vSubpassDep;
                std::vector<vk::ClearValue> vClearValues;

                //Framebuffer part
                std::vector<vk::Framebuffer> vFramebuffers;
                std::vector<FFramebufferAttachmentInfo> vFbAttachments;
                std::map<uint32_t, std::unordered_map<std::string, std::shared_ptr<CImage>>> mFramebufferImages;
                std::vector<std::shared_ptr<CImage>> vFramebufferDepth;
            };
        }
    }
}