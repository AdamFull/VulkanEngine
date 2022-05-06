#include "RenderPass.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/image/Image.h"
#include "graphics/VulkanInitializers.h"
#include "Subpass.h"

using namespace Engine::Core::Render;
using namespace Engine::Resources;
using namespace Engine::Core::Scene;

CRenderPass::Builder& CRenderPass::Builder::addAttachmentDescription(vk::AttachmentDescription&& desc)
{
    vk::ClearValue cv{};
    if(desc.finalLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        cv.setDepthStencil(vk::ClearDepthStencilValue{1.0f, 0});
    else
        cv.setColor(vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}});

    vClearValues.emplace_back(cv);
    vAttachDesc.emplace_back(std::move(desc));
    return *this;
}

CRenderPass::Builder& CRenderPass::Builder::addAttachmentDescription(const vk::Format format, vk::SampleCountFlagBits samples, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout)
{
    return addAttachmentDescription(std::move(vk::AttachmentDescription{{}, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout}));
}

CRenderPass::Builder& CRenderPass::Builder::addSubpassDescription(vk::SubpassDescription&& desc)
{
    vSubpassDesc.emplace_back(std::move(desc));
    return *this;
}

CRenderPass::Builder& CRenderPass::Builder::addSubpassDescription(vk::PipelineBindPoint bindPoint, const std::vector<vk::AttachmentReference>& attachRef, 
vk::AttachmentReference* depthAttach, const std::vector<vk::AttachmentReference>& inputRef)
{
    vk::SubpassDescription desc{};
    desc.pipelineBindPoint = bindPoint;
    desc.colorAttachmentCount = static_cast<uint32_t>(attachRef.size());
    desc.pColorAttachments = attachRef.data();
    desc.pDepthStencilAttachment = depthAttach;
    desc.inputAttachmentCount = static_cast<uint32_t>(inputRef.size());
    desc.pInputAttachments = inputRef.data();
    return addSubpassDescription(std::move(desc));
}

CRenderPass::Builder& CRenderPass::Builder::addSubpassDependency(vk::SubpassDependency&& dep)
{
    vSubpassDep.emplace_back(std::move(dep));
    return *this;
}

CRenderPass::Builder& CRenderPass::Builder::addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, 
vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::DependencyFlags depFlags)
{
    vk::SubpassDependency dep{};
    dep.srcSubpass = src;
    dep.dstSubpass = dst;
    dep.srcStageMask = srcStageMask;
    dep.dstStageMask = dstStageMask;
    dep.srcAccessMask = srcAccessMask;
    dep.dstAccessMask = dstAccessMask;
    dep.dependencyFlags = depFlags;
    return addSubpassDependency(std::move(dep));
}

std::unique_ptr<CRenderPass> CRenderPass::Builder::build()
{
    auto pRenderPass = std::make_unique<CRenderPass>();
    pRenderPass->vClearValues = std::move(vClearValues);
    pRenderPass->vAttachDesc = std::move(vAttachDesc);
    pRenderPass->vSubpassDesc = std::move(vSubpassDesc);
    pRenderPass->vSubpassDep = std::move(vSubpassDep);
    pRenderPass->renderPass = pRenderPass->createRenderPass();
    return pRenderPass;
}

CRenderPass::CRenderPass(vk::RenderPass&& pass) : renderPass(std::move(pass))
{

}

CRenderPass::~CRenderPass()
{
    CDevice::inst()->destroy(renderPass);
}

void CRenderPass::create()
{
    //Creating subpasses (render stages)
    currentSubpassIndex = 0;
    for(auto& subpass : vSubpasses)
    {
        subpass->create();
        currentSubpassIndex++;
    }
}

void CRenderPass::reCreate()
{
    renderPass = createRenderPass();
    currentSubpassIndex = 0;
    for(auto& subpass : vSubpasses)
    {
        subpass->reCreate();
        currentSubpassIndex++;
    }
}

void CRenderPass::cleanup()
{
}

void CRenderPass::begin(vk::CommandBuffer& commandBuffer)
{
    //Begins render pass for start rendering
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = CRenderSystem::inst()->getCurrentStage()->getFramebuffer()->getCurrentFramebuffer();
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(vClearValues.size());
    renderPassBeginInfo.pClearValues = vClearValues.data();
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport = Initializers::Viewport(renderArea.extent.width, renderArea.extent.height);
    vk::Rect2D scissor = renderArea;

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void CRenderPass::end(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.endRenderPass();
}

void CRenderPass::render(vk::CommandBuffer& commandBuffer)
{
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

void CRenderPass::setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height)
{
    setRenderArea(vk::Offset2D{offset_x, offset_y}, vk::Extent2D{width, height});
}

void CRenderPass::setRenderArea(vk::Offset2D offset, vk::Extent2D extent)
{
    setRenderArea(vk::Rect2D{offset, extent});
}

void CRenderPass::setRenderArea(vk::Rect2D&& area)
{
    renderArea = std::move(area);
}

void CRenderPass::pushSubpass(std::shared_ptr<CSubpass>&& subpass)
{
    vSubpasses.emplace_back(std::move(subpass));
}

vk::RenderPass CRenderPass::createRenderPass()
{
    vk::RenderPassCreateInfo renderPassCI = {};
    renderPassCI.attachmentCount = static_cast<uint32_t>(vAttachDesc.size());
    renderPassCI.pAttachments = vAttachDesc.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(vSubpassDesc.size());
    renderPassCI.pSubpasses = vSubpassDesc.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(vSubpassDep.size());
    renderPassCI.pDependencies = vSubpassDep.data();
    return CDevice::inst()->getLogical().createRenderPass(renderPassCI);
}