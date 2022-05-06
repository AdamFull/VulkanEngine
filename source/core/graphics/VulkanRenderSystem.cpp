#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/renderpass/render_stages/DeferredStage.h"
#include "graphics/renderpass/render_stages/PostProcessStage.h"
#include "graphics/renderpass/render_stages/PresentFinalStage.h"
#include "graphics/renderpass/render_stages/SandboxFinalStage.h"

using namespace Engine::Core;

template<>
std::unique_ptr<CRenderSystem> utl::singleton<CRenderSystem>::_instance{nullptr};

CRenderSystem::~CRenderSystem()
{
    cleanup();
}

void CRenderSystem::create()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CSwapChain::getInstance()->getFramesInFlight());

    vStages.emplace_back(std::make_unique<Render::CDeferredStage>());
    vStages.emplace_back(std::make_unique<Render::CPostProcessStage>());
    //vStages.emplace_back(std::make_unique<Render::CSandboxFinalStage>());
    vStages.emplace_back(std::make_unique<Render::CPresentFinalStage>());

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->create();
        currentStageIndex++;
    }
}

void CRenderSystem::reCreate()
{
    screenExtent = CSwapChain::getInstance()->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CSwapChain::getInstance()->getFramesInFlight());
    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->reCreate();
        currentStageIndex++;
    }
    Window::CWindowHandle::m_bWasResized = false;
}

void CRenderSystem::render()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
        catch (vk::OutOfDateKHRError err) { UHLInstance->recreateSwapChain(); }
        catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->render(commandBuffer);
        currentStageIndex++;
    }

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
    CSwapChain::getInstance()->acquireNextImage(&imageIndex);
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
