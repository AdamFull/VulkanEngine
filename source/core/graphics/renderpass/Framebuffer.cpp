#include "Framebuffer.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanStaticHelper.h"
#include "graphics/image/Image.h"
#include "graphics/image/ImageLoader.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

CFramebuffer::Builder& CFramebuffer::Builder::addImage(vk::Format format, vk::ImageUsageFlags usageFlags)
{
    attachments.emplace_back(FAttachmentInfo{format, usageFlags});
    return *this;
}

std::unique_ptr<CFramebuffer> CFramebuffer::Builder::build(vk::RenderPass &renderPass)
{
    auto pFramebuffer = std::make_unique<CFramebuffer>();
    pFramebuffer->attachments = std::move(attachments);
    return pFramebuffer;
}

CFramebuffer::CFramebuffer(std::vector<vk::Framebuffer>&& framebuffers)
{
    vFramebuffers = std::move(framebuffers);
}

CFramebuffer::~CFramebuffer()
{
    cleanup();
}

void CFramebuffer::create(vk::RenderPass& renderPass, vk::Extent2D extent)
{
    imagesExtent = extent;
    auto framesInFlight = USwapChain->GetFramesInFlight();
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for(auto& attachment : attachments)
        {
            if(attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                if(!pDepth)
                    pDepth = createImage(attachment.format, attachment.usageFlags, extent);
            }
            else
            {
                if(attachment.format == USwapChain->GetImageFormat())
                {
                    imageViews.push_back(USwapChain->GetImageViews()[frame]);
                }
                else
                {
                    mImages[frame].emplace_back(createImage(attachment.format, attachment.usageFlags, extent));
                    imageViews.push_back(mImages[frame].back()->GetDescriptor().imageView);
                }
            }
        }

        imageViews.push_back(pDepth->GetDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        vFramebuffers.emplace_back(UDevice->Make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI));
    }
}

void CFramebuffer::reCreate(vk::RenderPass& renderPass)
{
    //TODO: re create ops
    cleanup();
    create(renderPass, imagesExtent);
}

void CFramebuffer::cleanup()
{
    mImages.clear();
    for(auto& fb : vFramebuffers)
        UDevice->Destroy(fb);
}

std::shared_ptr<Image> CFramebuffer::createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent)
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
    /*if(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        texture->TransitionImageLayout(imageLayout, aspectMask, false);
    else*/
        texture->SetImageLayout(imageLayout);

    texture->UpdateDescriptor();
    return texture;
}