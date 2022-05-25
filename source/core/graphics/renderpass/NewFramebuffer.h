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

            enum class ERPAttachmentType
            {
                eInput,
                eOutput,
                eDepth
            };

            enum class EBarrierType
            {
                eBeginWithDepthWrite,
                eEndWithDepthWrite
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

                template<class... _Args>
                void addInputReference(uint32_t index, _Args... args) 
                {
                    std::array<uint32_t, sizeof...(_Args)> unpacked{std::forward<_Args>(args)...};  
                    std::vector<vk::AttachmentReference> references;
                    for(auto& arg : unpacked)
                        references.emplace_back(vk::AttachmentReference{arg, vk::ImageLayout::eShaderReadOnlyOptimal});
                    mInputReferences.emplace(index, references);
                }

                template<class... _Args>
                void addOutputReference(uint32_t index, _Args... args) 
                {
                    std::array<uint32_t, sizeof...(_Args)> unpacked{std::forward<_Args>(args)...};  
                    std::vector<vk::AttachmentReference> references;
                    for(auto& arg : unpacked)
                        references.emplace_back(vk::AttachmentReference{arg, vk::ImageLayout::eColorAttachmentOptimal});
                    mOutputReferences.emplace(index, references);
                }

                void addDescription(uint32_t subpass, bool bUseDepth = false);
                void addBarrier(uint32_t src, uint32_t dst, EBarrierType type);
                //Temporary while thinking about barrier generation
                void addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput, 
                    vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlags srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite, 
                    vk::AccessFlags dstAccessMask = vk::AccessFlagBits::eMemoryRead, vk::DependencyFlags depFlags = vk::DependencyFlagBits::eByRegion);

                void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
                void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
                void setRenderArea(vk::Rect2D &&area);

                void setFlipViewport(vk::Bool32 value) { flipViewport = value; }

                void pushSubpass(std::shared_ptr<CSubpass>&& subpass);
                const uint32_t getSubpassCount() const { return vSubpasses.size(); }
                vk::SubpassDescription& getCurrentDescription() { return vSubpassDesc.at(currentSubpassIndex); }
                uint32_t getCurrentSubpass() { return currentSubpassIndex; }
                vk::RenderPass& getRenderPass() { return renderPass; }

                //Framebuffer part
                void addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags);

                vk::Framebuffer& getFramebuffer(uint32_t index) { return vFramebuffers[index]; }
                vk::Framebuffer& getCurrentFramebuffer();
                std::unordered_map<std::string, std::shared_ptr<CImage>>& getImages(uint32_t index) { return mFramebufferImages[index]; }
                std::unordered_map<std::string, std::shared_ptr<CImage>>& getCurrentImages();
                std::shared_ptr<CImage>& getDepthImage() { return vFramebufferDepth.front(); }
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
                std::map<uint32_t, std::vector<vk::AttachmentReference>> mInputReferences;
                std::map<uint32_t, std::vector<vk::AttachmentReference>> mOutputReferences;
                vk::AttachmentReference depthReference;

                //Framebuffer part
                std::vector<vk::Framebuffer> vFramebuffers;
                std::vector<FFramebufferAttachmentInfo> vFbAttachments;
                std::map<uint32_t, std::unordered_map<std::string, std::shared_ptr<CImage>>> mFramebufferImages;
                std::vector<std::shared_ptr<CImage>> vFramebufferDepth;
            };
        }
    }
}