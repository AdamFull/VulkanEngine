#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/renderpass/render_stages/DeferredStage.h"
#include "graphics/renderpass/render_stages/PostProcessStage.h"
#include "graphics/renderpass/render_stages/PresentFinalStage.h"
#include "graphics/renderpass/render_stages/SandboxFinalStage.h"

using namespace engine::core;
using namespace engine::core::window;
using namespace engine::core::render;

template<>
scope_ptr<CRenderSystem> utl::singleton<CRenderSystem>::_instance{nullptr};

CRenderSystem::~CRenderSystem()
{
    cleanup();
}

void CRenderSystem::create()
{
    auto engineMode = UHLInstance->getCI().engine.mode;
    screenExtent = CDevice::inst()->getExtent();
    commandBuffers = make_ref<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CDevice::inst()->getFramesInFlight());

    vStages.emplace_back(make_scope<CDeferredStage>());
    vStages.emplace_back(make_scope<CPostProcessStage>());
    switch (engineMode)
    {
    case ELaunchMode::eGame:{
        vStages.emplace_back(make_scope<CPresentFinalStage>());
    } break;
    case ELaunchMode::eEditor:{
        vStages.emplace_back(make_scope<CSandboxFinalStage>());
    } break;
    }

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->create();
        currentStageIndex++;
    }

    updateFramebufferImages();
}

void CRenderSystem::reCreate()
{
    CDevice::inst()->tryRebuildSwapchain();
    screenExtent = CDevice::inst()->getExtent();
    commandBuffers = make_ref<CCommandBuffer>(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, CDevice::inst()->getFramesInFlight());
    imageIndex = 0;
    CWindowHandle::m_bWasResized = false;

    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->reCreate();
        currentStageIndex++;
    }
    updateFramebufferImages();
}

void CRenderSystem::rebuildViewport()
{
    CDevice::inst()->GPUWait();
    
    currentStageIndex = 0;
    for(auto& stage : vStages)
    {
        stage->rebuild();
        currentStageIndex++;
    }
    updateFramebufferImages();
}

void CRenderSystem::render()
{
    if(CDevice::inst()->getReduildFlag()) { reCreate(); }
    if(CDevice::inst()->isNeedToRebuildViewport()) { rebuildViewport(); }
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

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || CWindowHandle::m_bWasResized)
    {
        CDevice::inst()->setRebuildFlag();
    }

    if(totalFrameNumberCounter != std::numeric_limits<size_t>::max())
        totalFrameNumberCounter++;
    else
        totalFrameNumberCounter = 1;

    CDevice::inst()->updateCommandPools();
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

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, imageIndex);
    return commandBuffers->getCommandBuffer();
}

vk::Result CRenderSystem::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}

void CRenderSystem::updateFramebufferImages()
{
    mImages.clear();
    auto framesInFlight = CDevice::inst()->getFramesInFlight();
    for(auto& stage : vStages)
    {
        for(uint32_t fb = 0; fb < stage->getFramebufferCount(); fb++)
        {
            auto& framebuffer = stage->getFramebuffer(fb);
            for(uint32_t frame = 0; frame < framesInFlight; frame++)
            {
                auto& thisImages = mImages[frame];
                for(auto& [name, image] : framebuffer->getImages(frame))
                {
                    auto foundImage = thisImages.find(name);
                    if(foundImage != thisImages.end())
                        throw std::runtime_error("Image already exists.");
                    thisImages.emplace(name, image);
                }
            }
        }
    }
}