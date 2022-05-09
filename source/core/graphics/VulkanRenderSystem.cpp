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
    screenExtent = CDevice::inst()->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CDevice::inst()->getFramesInFlight());

    vStages.emplace_back(std::make_unique<Render::CDeferredStage>());
    vStages.emplace_back(std::make_unique<Render::CPostProcessStage>());
    vStages.emplace_back(std::make_unique<Render::CSandboxFinalStage>());
    //vStages.emplace_back(std::make_unique<Render::CPresentFinalStage>());

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->create();
        currentStageIndex++;
    }
}

void CRenderSystem::reCreate()
{
    CDevice::inst()->tryRebuildSwapchain();
    screenExtent = CDevice::inst()->getExtent();
    commandBuffers = std::make_shared<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CDevice::inst()->getFramesInFlight());
    imageIndex = 0;
    Window::CWindowHandle::m_bWasResized = false;

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->reCreate();
        currentStageIndex++;
    }
}

void CRenderSystem::render()
{
    if(CDevice::inst()->getReduildFlag()) { reCreate(); }
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { CDevice::inst()->setRebuildFlag(); }
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    if(!commandBuffer)
        return;

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
        CDevice::inst()->setRebuildFlag();
    }

    if(totalFrameNumberCounter != std::numeric_limits<size_t>::max())
        totalFrameNumberCounter++;
    else
        totalFrameNumberCounter = 1;
}

void CRenderSystem::cleanup()
{
    if(!bIsClean)
    {
        commandBuffers->cleanup();
        for(auto& stage : vStages)
            stage->cleanup();
        bIsClean = true;
    }
}

vk::CommandBuffer& CRenderSystem::getCurrentCommandBuffer()
{
    return commandBuffers->getCommandBuffer();
}

vk::CommandBuffer CRenderSystem::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    vk::Result res = CDevice::inst()->acquireNextImage(&imageIndex);
    if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR)
    {
        CDevice::inst()->setRebuildFlag();
        return nullptr;
    }
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
