#include "Framebuffer.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanStaticHelper.h"
#include "graphics/image/Image.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageLoader.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;

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
    auto framesInFlight = CSwapChain::getInstance()->getFramesInFlight();
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
                if(attachment.format == CSwapChain::getInstance()->getImageFormat())
                {
                    imageViews.push_back(CSwapChain::getInstance()->getImageViews()[frame]);
                }
                else
                {
                    auto image = createImage(attachment.format, attachment.usageFlags, extent);
                    mImages[frame].emplace(attachment.name, image);
                    imageViews.push_back(image->getDescriptor().imageView);
                }
            }
        }

        if(pDepth)
            imageViews.push_back(pDepth->getDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        vFramebuffers.emplace_back(CDevice::getInstance()->make<vk::Framebuffer, vk::FramebufferCreateInfo>(framebufferCI));
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
        CDevice::getInstance()->destroy(fb);
}

void CFramebuffer::addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags)
{
    attachments.emplace_back(FTextureAttachmentInfo{name, format, usageFlags});
}

vk::Framebuffer& CFramebuffer::getCurrentFramebuffer()
{
    return getFramebuffer(CSwapChain::getInstance()->getCurrentFrame());
}

std::unordered_map<std::string, std::shared_ptr<CImage>>& CFramebuffer::getCurrentImages()
{
    return getImages(CSwapChain::getInstance()->getCurrentFrame());
}

std::shared_ptr<CImage> CFramebuffer::createImage(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent2D extent)
{
    auto texture = std::make_shared<CImage2D>();
    bool translate_layout{false};

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
    else
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eGeneral;
        translate_layout = true;
    }

    texture->create(extent, format, imageLayout, usageFlags, aspectMask, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, 
    vk::SampleCountFlagBits::e1, translate_layout);
    return texture;
}