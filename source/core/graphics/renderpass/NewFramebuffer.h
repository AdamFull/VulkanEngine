#pragma once
#include "Subpass.h"
#include "graphics/image/Image.h"
#include "fifo_map.hpp"

namespace engine
{
    namespace core
    {
        namespace render
        {
            struct FFramebufferAttachmentInfo
            {
                vk::Format format;
                vk::ImageUsageFlags usageFlags;
                EImageType eType;
                uint32_t reference;
                uint32_t layers;
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

            class CFramebufferNew
            {
            public:
                ~CFramebufferNew();

                // Common part
                void create();
                void reCreate();

                // Renderpass part
                void begin(vk::CommandBuffer &commandBuffer);
                void end(vk::CommandBuffer &commandBuffer);
                void render(vk::CommandBuffer &commandBuffer);

                template <class... _Args>
                void addInputReference(uint32_t index, _Args... args)
                {
                    std::array<std::string, sizeof...(_Args)> unpacked{std::forward<_Args>(args)...};
                    std::vector<vk::AttachmentReference> references;
                    for (auto &arg : unpacked)
                    {
                        auto attachment = mFbAttachments.find(arg);
                        if(attachment != mFbAttachments.end())
                            references.emplace_back(vk::AttachmentReference{attachment->second.reference, vk::ImageLayout::eShaderReadOnlyOptimal});
                        else
                            assert(false && "Attachment not found.");
                    }   
                    mInputReferences.emplace(index, references);
                }

                template <class... _Args>
                void addOutputReference(uint32_t index, _Args... args)
                {
                    std::array<std::string, sizeof...(_Args)> unpacked{std::forward<_Args>(args)...};
                    std::vector<vk::AttachmentReference> references;
                    for (auto &arg : unpacked)
                    {
                        auto attachment = mFbAttachments.find(arg);
                        if(attachment != mFbAttachments.end())
                            references.emplace_back(vk::AttachmentReference{attachment->second.reference, vk::ImageLayout::eColorAttachmentOptimal});
                        else
                            assert(false && "Attachment not found.");
                    }
                    mOutputReferences.emplace(index, references);
                }

                void addDescription(uint32_t subpass, const std::string& depthReference = "");
                void addBarrier(uint32_t src, uint32_t dst, EBarrierType type);
                // Temporary while thinking about barrier generation
                void addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                          vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlags srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
                                          vk::AccessFlags dstAccessMask = vk::AccessFlagBits::eMemoryRead, vk::DependencyFlags depFlags = vk::DependencyFlagBits::eByRegion);

                void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
                void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
                void setRenderArea(vk::Rect2D &&area);

                void setFlipViewport(vk::Bool32 value) { flipViewport = value; }

                void addRenderer(utl::scope_ptr<CSubpass> &&subpass);
                utl::scope_ptr<CSubpass>& getCurrentRenderer() { return vSubpasses.at(currentSubpassIndex); }

                const size_t getSubpassCount() const { return vSubpasses.size(); }
                vk::SubpassDescription &getCurrentDescription() { return vSubpassDesc.at(currentSubpassIndex); }
                uint32_t getCurrentSubpass() { return currentSubpassIndex; }
                vk::RenderPass &getRenderPass() { return renderPass; }

                // Framebuffer part
                void addImage(const std::string &name, vk::Format format, vk::ImageUsageFlags usageFlags, EImageType eImageType = EImageType::e2D, uint32_t layers = 1);

                vk::Framebuffer &getFramebuffer(uint32_t index) { return vFramebuffers[index]; }
                vk::Framebuffer &getCurrentFramebuffer();
                std::unordered_map<std::string, utl::ref_ptr<CImage>> &getImages(uint32_t index) { return mFramebufferImages[index]; }
                std::unordered_map<std::string, utl::ref_ptr<CImage>> &getCurrentImages();
                utl::ref_ptr<CImage> &getDepthImage() { return vFramebufferDepth.front(); }

            private:
                void createRenderPass();
                void createFramebuffer();
                static utl::ref_ptr<CImage> createImage(const FFramebufferAttachmentInfo& attachment, vk::Extent2D extent);

                uint32_t getCurrentFrameProxy();
                static bool isColorAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eColorAttachment); }
                static bool isDepthAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment); }
                static bool isSampled(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eSampled); }
                static bool isInputAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eInputAttachment); }

            private:
                // Common part
                vk::Rect2D renderArea;

                // Renderpass part
                vk::RenderPass renderPass{nullptr};
                std::vector<utl::scope_ptr<CSubpass>> vSubpasses;
                vk::Bool32 flipViewport{VK_FALSE};
                uint32_t currentSubpassIndex{0};

                std::vector<vk::AttachmentDescription> vAttachDesc;
                std::vector<vk::SubpassDescription> vSubpassDesc;
                std::vector<vk::SubpassDependency> vSubpassDep;
                std::vector<vk::ClearValue> vClearValues;
                std::map<uint32_t, std::vector<vk::AttachmentReference>> mInputReferences;
                std::map<uint32_t, std::vector<vk::AttachmentReference>> mOutputReferences;
                std::map<std::string, vk::AttachmentReference> mDepthReference;

                // Framebuffer part
                std::vector<vk::Framebuffer> vFramebuffers;
                nlohmann::fifo_map<std::string, FFramebufferAttachmentInfo> mFbAttachments;
                std::map<uint32_t, std::unordered_map<std::string, utl::ref_ptr<CImage>>> mFramebufferImages;
                std::vector<utl::ref_ptr<CImage>> vFramebufferDepth;
            };
        }
    }
}