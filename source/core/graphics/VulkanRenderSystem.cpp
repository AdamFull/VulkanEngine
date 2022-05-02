#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/renderpass/render_stages/DeferredStage.h"

using namespace Engine::Core;

RenderSystem::~RenderSystem()
{
    cleanup();
}

void RenderSystem::create(std::shared_ptr<Resources::CResourceManager> resourceManager, std::shared_ptr<Scene::Objects::RenderObject> root)
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());

    //UOverlay->Create(root, renderPass->get(), renderPass->getSubpassCount() - 1);

    creationBuffer = std::make_unique<Render::FRenderCreateInfo>();
    renderData = std::make_unique<Render::FRenderProcessInfo>();
    creationBuffer->resourceManager = resourceManager;
    creationBuffer->root = root;

    vStages.emplace_back(std::make_unique<Render::CDeferredStage>());

    for(auto& stage : vStages)
        stage->create(creationBuffer);
}

void RenderSystem::reCreate()
{
    screenExtent = USwapChain->GetExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, USwapChain->GetFramesInFlight());
}

void RenderSystem::render(std::shared_ptr<Scene::Objects::RenderObject> root)
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { UHLInstance->RecreateSwapChain(); }
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    renderData->commandBuffer = commandBuffer;
    renderData->root = root;

    for(auto& stage : vStages)
        stage->render(renderData);

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
    for(auto& stage : vStages)
        stage->cleanup();
}

vk::CommandBuffer& RenderSystem::getCurrentCommandBuffer()
{
    return commandBuffers->getCommandBuffer();
}

vk::CommandBuffer& RenderSystem::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    USwapChain->AcquireNextImage(&imageIndex);
    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse, imageIndex);
    return commandBuffers->getCommandBuffer();
}

vk::Result RenderSystem::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}
