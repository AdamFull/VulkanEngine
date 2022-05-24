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
    auto framesInFlight = CDevice::inst()->getFramesInFlight();
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for(auto& attachment : attachments)
        {
            if(attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                if(vDepth.empty())
                    vDepth.emplace_back(createImage(attachment.format, attachment.usageFlags, imagesExtent));
            }
            else
            {
                if(attachment.format == CDevice::inst()->getImageFormat())
                {
                    imageViews.push_back(CDevice::inst()->getImageViews()[frame]);
                }
                else
                {
                    auto image = createImage(attachment.format, attachment.usageFlags, imagesExtent);
                    mImages[frame].emplace(attachment.name, image);
                    imageViews.push_back(image->getDescriptor().imageView);
                }
            }
        }

        if(!vDepth.empty())
            imageViews.push_back(vDepth.front()->getDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        vk::Framebuffer framebuffer{VK_NULL_HANDLE};
        vk::Result res = CDevice::inst()->create(framebufferCI, &framebuffer);
        assert(res == vk::Result::eSuccess && "Cannot create framebuffer.");
        vFramebuffers.emplace_back(framebuffer);
    }
}

void CFramebuffer::reCreate(vk::RenderPass& renderPass, vk::Extent2D extent)
{
    vFramebuffers.clear();
    mImages.clear();
    vDepth.clear();
    create(renderPass, extent);
}

void CFramebuffer::cleanup()
{
    if(!bIsClean)
    {
        for(auto& framebuffer : vFramebuffers)
            CDevice::inst()->destroy(&framebuffer);
        vFramebuffers.clear();
        mImages.clear();
        vDepth.clear();
        bIsClean = true;
    }   
}

void CFramebuffer::addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags)
{
    attachments.emplace_back(FTextureAttachmentInfo{name, format, usageFlags});
}

vk::Framebuffer& CFramebuffer::getCurrentFramebuffer()
{
    return getFramebuffer(CDevice::inst()->getCurrentFrame());
}

std::unordered_map<std::string, std::shared_ptr<CImage>>& CFramebuffer::getCurrentImages()
{
    return getImages(CDevice::inst()->getCurrentFrame());
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

    texture->create(extent, format, imageLayout, usageFlags, aspectMask, vk::Filter::eNearest, vk::SamplerAddressMode::eRepeat, 
    vk::SampleCountFlagBits::e1, translate_layout);
    return texture;
}