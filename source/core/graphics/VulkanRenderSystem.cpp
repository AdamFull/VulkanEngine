#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/RenderPass.hpp"
#include "graphics/renderpass/subpasses/GBufferPass.h"
#include "graphics/renderpass/subpasses/PBRCompositionPass.h"
#include "graphics/renderpass/subpasses/GaussianBlurPass.h"
#include "graphics/renderpass/subpasses/FinalCompositionPass.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace Engine::Core;

RenderSystem::~RenderSystem()
{
    cleanup();
}

void RenderSystem::create(std::shared_ptr<Resources::ResourceManager> resourceManager, std::shared_ptr<Scene::Objects::RenderObject> root)
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());

    std::vector<vk::AttachmentReference> vReferences_0
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal},
        {1, vk::ImageLayout::eColorAttachmentOptimal},
        {2, vk::ImageLayout::eColorAttachmentOptimal},
        {3, vk::ImageLayout::eColorAttachmentOptimal},
        {4, vk::ImageLayout::eColorAttachmentOptimal},
        {5, vk::ImageLayout::eColorAttachmentOptimal},
        {6, vk::ImageLayout::eColorAttachmentOptimal}
    };
    vk::AttachmentReference depthReference{8, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    /*[E] 2022-04-29 21:32:35.7899637 GMT+3 | VulkanDevice.cpp:ValidationCallback:48 | 
    Validation Error: [ VUID-VkSubpassDescription-None-04437 ] Object 0: handle = 0x12f71a69360, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x438c2f01 | 
    vkCreateRenderPass(): pSubpasses[1].pColorAttachments[1] is also an input attachment so the layout (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) must not be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL or VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR. 
    The Vulkan spec states: Each attachment must follow the image layout requirements specified for its attachment type (https://vulkan.lunarg.com/doc/view/1.2.198.1/windows/1.2-extensions/vkspec.html#VUID-VkSubpassDescription-None-04437)
*/

    std::vector<vk::AttachmentReference> vReferences_1
    {
        {0, vk::ImageLayout::eColorAttachmentOptimal},
        {7, vk::ImageLayout::eColorAttachmentOptimal}
    };

    std::vector<vk::AttachmentReference> vInputReferences_1
    {
        {1, vk::ImageLayout::eShaderReadOnlyOptimal},
        {2, vk::ImageLayout::eShaderReadOnlyOptimal},
        {3, vk::ImageLayout::eShaderReadOnlyOptimal},
        {4, vk::ImageLayout::eShaderReadOnlyOptimal},
        {5, vk::ImageLayout::eShaderReadOnlyOptimal},
        {6, vk::ImageLayout::eShaderReadOnlyOptimal}
    };

    auto renderPass = Render::CRenderPass::Builder().
    //KHR color attachment
    addAttachmentDescription(USwapChain->GetImageFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR).
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Position buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Light mask buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Normals buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Albedo buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Emissive buffer
    addAttachmentDescription(vk::Format::eR8G8B8A8Snorm). //Metal, roughness, AmbientOcclusion, Height maps buffer
    addAttachmentDescription(vk::Format::eR16G16B16A16Sfloat). //Temporary image buffer
    addAttachmentDescription(Image::GetDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal). //Depth stencil
    //GBuffer description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_0, &depthReference).
    //PBR composition description
    addSubpassDescription(vk::PipelineBindPoint::eGraphics, vReferences_1, &depthReference, vInputReferences_1).
    //Begin drawing gbuffer
    addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite).
    //Begin drawing first composition
    addSubpassDependency(0, 1).
    //Draw final composition
    addSubpassDependency(1, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead).
    build();

    renderPass->pushSubpass(std::make_shared<Render::CGBufferPass>());
    renderPass->pushSubpass(std::make_shared<Render::CPBRCompositionPass>());
    //renderPass->pushSubpass(std::make_shared<Render::CGaussianBlurPass>(1));
    //renderPass->pushSubpass(std::make_shared<Render::CGaussianBlurPass>(-1));
    //renderPass->pushSubpass(std::make_shared<Render::CFinalCompositionPass>());

    auto fbBuilder = Render::CFramebuffer::Builder();
    for(auto& attach : renderPass->getAttachments())
        fbBuilder = fbBuilder.addImage(attach.format, attach.usageFlags);
    auto framebuffer = fbBuilder.build(renderPass->get());

    //UOverlay->Create(root, renderPass->get(), renderPass->getSubpassCount() - 1);

    mRenderers.emplace(std::move(renderPass), std::move(framebuffer));

    for(auto& [pass, fb] : mRenderers)
    {
        pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        fb->create(pass->get(), screenExtent);
        pass->create(resourceManager, fb->getImages(imageIndex), root);
    }
}

void RenderSystem::reCreate()
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
    for(auto& [pass, fb] : mRenderers)
    {
        pass->setRenderArea(vk::Offset2D{0, 0}, screenExtent);
        pass->reCreate();
        fb->reCreate(pass->get());
    }
}

void RenderSystem::render(std::shared_ptr<Scene::Objects::RenderObject> root)
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { UHLInstance->RecreateSwapChain(); }
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    for(auto& [pass, fb] : mRenderers)
    {
        pass->begin(commandBuffer, fb->get());
        pass->render(commandBuffer, fb->getImages(imageIndex), root);
        pass->end(commandBuffer);
    }

    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { throw std::runtime_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || Window::WindowHandle::m_bWasResized)
    {
        Window::WindowHandle::m_bWasResized = false;
        UHLInstance->RecreateSwapChain();
    }
}

void RenderSystem::cleanup()
{
    for(auto& [pass, fb] : mRenderers)
    {
        pass->cleanup();
        fb->cleanup();
    }
}

vk::CommandBuffer& RenderSystem::getCurrentCommandBuffer()
{
    return commandBuffers->GetCommandBuffer();
}

vk::CommandBuffer& RenderSystem::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    USwapChain->AcquireNextImage(&imageIndex);
    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse, imageIndex);
    return commandBuffers->GetCommandBuffer();
}

vk::Result RenderSystem::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}
