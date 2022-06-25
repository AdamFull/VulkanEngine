#include "NewFramebuffer.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/Image2DArray.h"
#include "graphics/image/Image3D.h"
#include "graphics/image/ImageCubemap.h"

using namespace engine::core;
using namespace engine::core::render;

void CFramebufferNew::create()
{
    //Looking for depth attachment
    uint32_t depthIndex{};
    for(auto& attachment : vFbAttachments)
    {
        if(isDepthAttachment(attachment.usageFlags))
            depthReference = vk::AttachmentReference{depthIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal};
        depthIndex++;
    }

    createRenderPass();
    createFramebuffer();

    currentSubpassIndex = 0;
    for(auto& subpass : vSubpasses)
    {
        subpass->create();
        currentSubpassIndex++;
    }
}

void CFramebufferNew::reCreate()
{
    CDevice::inst()->destroy(&renderPass);
    createRenderPass();
    currentSubpassIndex = 0;
    for(auto& subpass : vSubpasses)
    {
        subpass->reCreate();
        currentSubpassIndex++;
    }

    vFramebuffers.clear();
    mFramebufferImages.clear();
    vFramebufferDepth.clear();
    createFramebuffer();
}

void CFramebufferNew::cleanup()
{
    if(!bIsClean)
    {
        for(auto& subpass : vSubpasses)
            subpass->cleanup();
        vSubpasses.clear();
        if(renderPass)
            CDevice::inst()->destroy(&renderPass);
        bIsClean = true;

        for(auto& framebuffer : vFramebuffers)
            CDevice::inst()->destroy(&framebuffer);
        vFramebuffers.clear();
        mFramebufferImages.clear();
        vFramebufferDepth.clear();
    }
}

void CFramebufferNew::begin(vk::CommandBuffer &commandBuffer)
{
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = getCurrentFramebuffer();
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(vClearValues.size());
    renderPassBeginInfo.pClearValues = vClearValues.data();
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    if(flipViewport)
    {
        viewport.width = static_cast<float>(renderArea.extent.width);
        viewport.height = -static_cast<float>(renderArea.extent.height);
        viewport.x = 0;   
        viewport.y = static_cast<float>(renderArea.extent.height);
    }
    else
    {
        viewport.width = renderArea.extent.width;
        viewport.height = renderArea.extent.height;
        viewport.x = 0;
        viewport.y = 0;
    }
    
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vk::Rect2D scissor = renderArea;

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void CFramebufferNew::end(vk::CommandBuffer &commandBuffer)
{
    commandBuffer.endRenderPass();
}

void CFramebufferNew::render(vk::CommandBuffer& commandBuffer)
{
    for(auto& subpass : vSubpasses)
        subpass->beforeRender(commandBuffer);

    begin(commandBuffer);
    //Render each stage
    currentSubpassIndex = 0;
    for(auto& subpass : vSubpasses)
    {
        subpass->render(commandBuffer);
        if((vSubpasses.size() - 1) > currentSubpassIndex)
            commandBuffer.nextSubpass(vk::SubpassContents::eInline);
        currentSubpassIndex++;
    }
    end(commandBuffer);
}

void CFramebufferNew::addDescription(uint32_t subpass, bool bUseDepth)
{
    
    vk::SubpassDescription description{};
    description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    auto attachmentRef = mOutputReferences.find(subpass);
    if(attachmentRef != mOutputReferences.end())
    {
        description.colorAttachmentCount = static_cast<uint32_t>(attachmentRef->second.size());
        description.pColorAttachments = attachmentRef->second.data();
    }
    
    auto inputRef = mInputReferences.find(subpass);
    if(inputRef != mInputReferences.end())
    {
        description.inputAttachmentCount = static_cast<uint32_t>(inputRef->second.size());
        description.pInputAttachments = inputRef->second.data();
    }
    description.pDepthStencilAttachment = bUseDepth ? &depthReference : nullptr;
    vSubpassDesc.emplace_back(description);
}

void CFramebufferNew::addBarrier(uint32_t src, uint32_t dst, EBarrierType type)
{
    vk::SubpassDependency dep{};
    dep.srcSubpass = src;
    dep.dstSubpass = dst;
    dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    switch (type)
    {
    case EBarrierType::eBeginWithDepthWrite: {
        dep.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
        dep.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }break;
    case EBarrierType::eEndWithDepthWrite: {
        dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dep.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    }break;
    }
}

void CFramebufferNew::addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, 
vk::AccessFlags dstAccessMask, vk::DependencyFlags depFlags)
{
    vk::SubpassDependency dep{};
    dep.srcSubpass = src;
    dep.dstSubpass = dst;
    dep.srcStageMask = srcStageMask;    //Pipeline stage what we waiting
    dep.dstStageMask = dstStageMask;    //Pipeline stage where we waiting
    dep.srcAccessMask = srcAccessMask;  //
    dep.dstAccessMask = dstAccessMask;
    dep.dependencyFlags = depFlags;
    vSubpassDep.emplace_back(dep);
}

void CFramebufferNew::setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height)
{
    setRenderArea(vk::Offset2D{offset_x, offset_y}, vk::Extent2D{width, height});
}

void CFramebufferNew::setRenderArea(vk::Offset2D offset, vk::Extent2D extent)
{
    setRenderArea(vk::Rect2D{offset, extent});
}

void CFramebufferNew::setRenderArea(vk::Rect2D&& area)
{
    renderArea = std::move(area);
}

void CFramebufferNew::addRenderer(scope_ptr<CSubpass>&& subpass)
{
    vSubpasses.emplace_back(std::move(subpass));
}

void CFramebufferNew::addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags, EImageType eImageType, uint32_t layers)
{
    vk::ClearValue clearValue{};
    vk::AttachmentDescription attachmentDescription{};
    attachmentDescription.format = format;
    attachmentDescription.samples = vk::SampleCountFlagBits::e1;
    attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;

    if(isColorAttachment(usageFlags))
    {
        if(isSampled(usageFlags) && !isInputAttachment(usageFlags))
        {
            attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if(isInputAttachment(usageFlags) && !isSampled(usageFlags))
        {
            attachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }
        else if(isInputAttachment(usageFlags) && isSampled(usageFlags))
        {
            attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else
        {
            if(format == CDevice::inst()->getImageFormat())
            {
                attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;
            }
            else
                assert(false && "Cannot use sampled image with input attachment.");
        }
        clearValue.setColor(vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}});
    }
    else if(isDepthAttachment(usageFlags))
    {
        attachmentDescription.loadOp = vk::AttachmentLoadOp::eDontCare;
        if(isSampled(usageFlags))
        {
            attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
        }
        else
            attachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        clearValue.setDepthStencil(vk::ClearDepthStencilValue{1.0f, 0});
    }

    vAttachDesc.emplace_back(attachmentDescription);
    vClearValues.emplace_back(clearValue);
    vFbAttachments.emplace_back(FFramebufferAttachmentInfo{name, format, usageFlags, eImageType, layers});
}

vk::Framebuffer& CFramebufferNew::getCurrentFramebuffer()
{
    return getFramebuffer(getCurrentFrameProxy());
}

std::unordered_map<std::string, ref_ptr<CImage>>& CFramebufferNew::getCurrentImages()
{
    return getImages(getCurrentFrameProxy());
}

void CFramebufferNew::createRenderPass()
{
    vk::RenderPassCreateInfo renderPassCI = {};
    renderPassCI.attachmentCount = static_cast<uint32_t>(vAttachDesc.size());
    renderPassCI.pAttachments = vAttachDesc.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(vSubpassDesc.size());
    renderPassCI.pSubpasses = vSubpassDesc.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(vSubpassDep.size());
    renderPassCI.pDependencies = vSubpassDep.data();
    vk::Result res = CDevice::inst()->create(renderPassCI, &renderPass);
    assert(res == vk::Result::eSuccess && "Cannot create render pass.");
}

void CFramebufferNew::createFramebuffer()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();
    for(size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for(auto& attachment : vFbAttachments)
        {
            if(attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                if(vFramebufferDepth.empty())
                    vFramebufferDepth.emplace_back(createImage(attachment, renderArea.extent));
                mFramebufferImages[frame].emplace(attachment.name, vFramebufferDepth.back());
            }
            else
            {
                if(attachment.format == CDevice::inst()->getImageFormat())
                {
                    imageViews.push_back(CDevice::inst()->getImageViews()[frame]);
                }
                else
                {
                    auto image = createImage(attachment, renderArea.extent);
                    mFramebufferImages[frame].emplace(attachment.name, image);
                    imageViews.push_back(image->getDescriptor().imageView);
                }
            }
        }

        if(!vFramebufferDepth.empty())
            imageViews.push_back(vFramebufferDepth.back()->getDescriptor().imageView);

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = renderArea.extent.width;
        framebufferCI.height = renderArea.extent.height;
        framebufferCI.layers = 1;

        vk::Framebuffer framebuffer{VK_NULL_HANDLE};
        vk::Result res = CDevice::inst()->create(framebufferCI, &framebuffer);
        assert(res == vk::Result::eSuccess && "Cannot create framebuffer.");
        vFramebuffers.emplace_back(framebuffer);
    }
}

ref_ptr<CImage> CFramebufferNew::createImage(const FFramebufferAttachmentInfo& attachment, vk::Extent2D extent)
{
    ref_ptr<CImage> texture;

    switch (attachment.eType)
    {
    case EImageType::e2D: texture = make_ref<CImage2D>(); break;
    case EImageType::e3D: texture = make_ref<CImage3D>(); break;
    case EImageType::eArray2D: texture = make_ref<CImage2DArray>(); break;
    case EImageType::eArray3D: break;
    case EImageType::eCubeMap: texture = make_ref<CImageCubemap>(); break;
    case EImageType::eArrayCube: break;
    }

    bool translate_layout{false};

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if(isColorAttachment(attachment.usageFlags))
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    else if(isDepthAttachment(attachment.usageFlags))
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        if(isSampled(attachment.usageFlags))
            imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
        else
            imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }
    else
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eGeneral;
        translate_layout = true;
    }

    switch (attachment.eType)
    {
    case EImageType::e2D: {
        auto tex2d = std::dynamic_pointer_cast<CImage2D>(texture);
        tex2d->create(extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eNearest, vk::SamplerAddressMode::eRepeat, vk::SampleCountFlagBits::e1, translate_layout);
    } break;
    case EImageType::e3D: {
        //TODO: Add implementation
    } break;
    case EImageType::eArray2D: {
        auto tex2darr = std::dynamic_pointer_cast<CImage2DArray>(texture);
        tex2darr->create(attachment.layers, extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eNearest, vk::SamplerAddressMode::eRepeat, vk::SampleCountFlagBits::e1, translate_layout);
    } break;
    case EImageType::eArray3D: {
        //TODO: Add implementation
    } break;
    case EImageType::eCubeMap: {
        //TODO: Add implementation
    } break;
    case EImageType::eArrayCube: {
        //TODO: Add implementation
    } break;
    }

    return texture;
}

uint32_t CFramebufferNew::getCurrentFrameProxy()
{
    return CDevice::inst()->getCurrentFrame();
}

//Finish generation descriptions
//Finish generation barriers
