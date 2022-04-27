#include "Framebuffer.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanStaticHelper.h"
#include "graphics/image/Image.h"
#include "graphics/image/ImageLoader.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CFramebuffer::Builder& CFramebuffer::Builder::addImage(vk::Format format, vk::ImageUsageFlags usageFlags)
{
    /*if(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        depth = createImage(format, usageFlags, extent);
    else
    {
        for(auto frame = 0; frame < USwapChain->GetFramesInFlight(); frame++)
            images[frame].emplace_back(createImage(format, usageFlags, extent));
    }*/
    attachments.emplace_back(FAttachmentInfo{format, usageFlags});

    return *this;
}

std::shared_ptr<Image> CFramebuffer::Builder::createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent)
{
    auto texture = std::make_shared<Image>();
    ktxTexture *offscreen;
    vk::Format imageFormat;
    Loaders::ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, extent.width, extent.height, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(format));

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if(usageFlags & vk::ImageUsageFlagBits::eColorAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    else if(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }

    texture->InitializeTexture(offscreen, imageFormat, usageFlags, aspectMask);
    if(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        texture->TransitionImageLayout(imageLayout, aspectMask, false);
    else
        texture->SetImageLayout(imageLayout);

    texture->UpdateDescriptor();
    return texture;
}

std::unique_ptr<CFramebuffer> CFramebuffer::Builder::build(vk::RenderPass &renderPass, vk::Extent2D extent)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    std::vector<vk::Framebuffer> framebuffers{};
    framebuffers.resize(framesInFlight);
    std::map<uint32_t, std::vector<std::shared_ptr<Image>>> images;
    std::shared_ptr<Image> depth;

    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for(auto& attachment : attachments)
        {
            if(attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                if(!depth)
                    depth = createImage(attachment.format, attachment.usageFlags, extent);
            }
            else
            {
                images[frame].emplace_back(createImage(attachment.format, attachment.usageFlags, extent));
                imageViews.push_back(images[frame].back()->GetDescriptor().imageView);
            }
        }

        imageViews.push_back(depth->GetDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        framebuffers[frame] = UDevice->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI);
    }

    auto pFramebuffer = std::make_unique<CFramebuffer>(std::move(framebuffers));
    pFramebuffer->mImages = std::move(images);
    pFramebuffer->pDepth = std::move(depth);
    return pFramebuffer;
}

CFramebuffer::CFramebuffer(std::vector<vk::Framebuffer>&& framebuffers)
{
    vFramebuffers = std::move(framebuffers);
}