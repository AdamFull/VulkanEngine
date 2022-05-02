#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/renderpass/render_stages/DeferredStage.h"

using namespace Engine::Core;

CRenderSystem::~CRenderSystem()
{
    cleanup();
}

void CRenderSystem::create(std::shared_ptr<Resources::CResourceManager> resourceManager, std::shared_ptr<Scene::CRenderObject> root)
{
    screenExtent = USwapChain->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->getFramesInFlight());

    vStages.emplace_back(std::make_unique<Render::CDeferredStage>());

    for(auto& stage : vStages)
        stage->create(resourceManager, root);
}

void CRenderSystem::reCreate()
{
    screenExtent = USwapChain->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->getFramesInFlight());
}

void CRenderSystem::render(std::shared_ptr<Scene::CRenderObject> root)
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { UHLInstance->recreateSwapChain(); }
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    for(auto& stage : vStages)
        stage->render(commandBuffer, root);

    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { throw std::runtime_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || Window::CWindowHandle::m_bWasResized)
    {
        Window::CWindowHandle::m_bWasResized = false;
        UHLInstance->recreateSwapChain();
    }
}

void CRenderSystem::cleanup()
{
    for(auto& stage : vStages)
        stage->cleanup();
}

vk::CommandBuffer& CRenderSystem::getCurrentCommandBuffer()
{
    return commandBuffers->getCommandBuffer();
}

vk::CommandBuffer& CRenderSystem::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    USwapChain->acquireNextImage(&imageIndex);
    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse, imageIndex);
    return commandBuffers->getCommandBuffer();
}

vk::Result CRenderSystem::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}
