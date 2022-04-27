#include "VulkanRenderer.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/renderpass/RenderPass.hpp"

using namespace Engine::Core;

CRenderer::~CRenderer()
{
    cleanup();
}

void CRenderer::create()
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
    for(auto& pass : vRenderPasses)
        pass->create();
}

void CRenderer::reCreate()
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
    for(auto& pass : vRenderPasses)
        pass->reCreate();
}

void CRenderer::render()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { UHLInstance->RecreateSwapChain(); }
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    for(auto& pass : vRenderPasses)
        pass->render(commandBuffer);

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

void CRenderer::cleanup()
{
    for(auto& pass : vRenderPasses)
        pass->cleanup();
}

vk::CommandBuffer& CRenderer::getCurrentCommandBuffer()
{
    return commandBuffers->GetCommandBuffer();
}

vk::CommandBuffer& CRenderer::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    USwapChain->AcquireNextImage(&imageIndex);
    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse, imageIndex);
    return commandBuffers->GetCommandBuffer();
}

vk::Result CRenderer::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}
