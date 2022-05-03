#include "PBRCompositionPass.h"
#include "resources/materials/MaterialLoader.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/camera/CameraManager.h"
#include "graphics/scene/lightning/LightSourceManager.h"
#include "graphics/scene/objects/components/light/LightComponent.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanInitializers.h"
#include "graphics/pipeline/Pipeline.h"
#include "graphics/pipeline/ComputePipeline.h"
#include "graphics/descriptor/DescriptorHandler.h"
#include "graphics/image/ImageLoader.h"
#include "graphics/image/Image.h"
#include "graphics/buffer/PushHandler.hpp"
#include "GlobalVariables.h"

using namespace Engine::Core;
using namespace Engine::Core::Render;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Scene;


void CPBRCompositionPass::create(std::shared_ptr<Resources::CResourceManager>& resourceManager, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto framesInFlight = USwapChain->getFramesInFlight();
    pUniform = std::make_shared<CUniformBuffer>();
    pUniform->create(framesInFlight, sizeof(FLightningData));

    m_pSkybox = resourceManager->Get<CImage>("skybox_cubemap_tex");

    brdf = UHLInstance->getThreadPool()->submit(&CPBRCompositionPass::ComputeBRDFLUT, 512);
    irradiance = UHLInstance->getThreadPool()->submit(&CPBRCompositionPass::ComputeIrradiance, m_pSkybox, 64);
    prefiltered = UHLInstance->getThreadPool()->submit(&CPBRCompositionPass::ComputePrefiltered, m_pSkybox, 512);

    auto& renderPass = URenderer->getCurrentStage()->getRenderPass()->get();
    auto subpass = URenderer->getCurrentStage()->getRenderPass()->getCurrentSubpass();

    pMaterial = CMaterialLoader::getInstance()->create("pbr_composition");
    pMaterial->create(renderPass, subpass);

    UOverlay->create(root, renderPass, subpass);
    CSubpass::create(resourceManager, root);
}

void CPBRCompositionPass::render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root)
{
    auto& images = URenderer->getCurrentStage()->getFramebuffer()->getCurrentImages();
    pMaterial->addTexture("brdflut_tex", *brdf);
    pMaterial->addTexture("irradiance_tex", *irradiance);
    pMaterial->addTexture("prefiltred_tex", *prefiltered);
    pMaterial->addTexture("position_tex", images["position_tex"]);
    pMaterial->addTexture("lightning_mask_tex", images["lightning_mask_tex"]);
    pMaterial->addTexture("normal_tex", images["normal_tex"]);
    pMaterial->addTexture("albedo_tex", images["albedo_tex"]);
    pMaterial->addTexture("emission_tex", images["emission_tex"]);
    pMaterial->addTexture("mrah_tex", images["mrah_tex"]);

    auto imageIndex = USwapChain->getCurrentFrame();

    //May be move to CompositionObject
    FLightningData ubo;
    auto camera = CCameraManager::getInstance()->getCurrentCamera();
    auto vLights = CLightSourceManager::getInstance()->getSources();
    for(std::size_t i = 0; i < vLights.size(); i++)
        ubo.lights[i] = vLights.at(i);

    ubo.lightCount = vLights.size();
    ubo.viewPos = camera->viewPos; //camera->viewPos
    ubo.bloomThreshold = GlobalVariables::bloomThreshold;
    
    pUniform->updateUniformBuffer(imageIndex, &ubo);
    auto& buffer = pUniform->getUniformBuffer(imageIndex);
    auto descriptor = buffer->getDscriptor();
    pMaterial->addBuffer("UBOLightning", descriptor);
    pMaterial->update(imageIndex);
    pMaterial->bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);

    UOverlay->drawFrame(commandBuffer, imageIndex);
}

void CPBRCompositionPass::cleanup()
{
    CSubpass::cleanup();
}

std::shared_ptr<CImage> CPBRCompositionPass::ComputeBRDFLUT(uint32_t size)
{
    auto brdfImage = std::make_shared<CImage>();
    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::CImageLoader::allocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR16G16Sfloat));
    brdfImage->initializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    brdfImage->transitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    brdfImage->updateDescriptor();

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    auto descriptor = Descriptor::CDescriptorHandler();

    std::shared_ptr<Pipeline::CPipelineBase> computePipeline = Pipeline::CPipelineBase::Builder().
    addShaderStage("shaders/generators/brdflut.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    computePipeline->bind(commandBuffer);

    descriptor.create(computePipeline);
    descriptor.set("outColour", brdfImage->getDescriptor());
    descriptor.update(0);
    descriptor.bind(commandBuffer, 0);
    
    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    Loaders::CImageLoader::close(&offscreen);

    return brdfImage;
}

std::shared_ptr<CImage> CPBRCompositionPass::ComputeIrradiance(const std::shared_ptr<CImage> &source, uint32_t size)
{
    auto irradianceCubemap = std::make_shared<CImage>();

    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::CImageLoader::allocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR32G32B32A32Sfloat));
    offscreen->isCubemap = true;
    irradianceCubemap->initializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    irradianceCubemap->transitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    irradianceCubemap->updateDescriptor();

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    auto descriptor = Descriptor::CDescriptorHandler();
    
    std::shared_ptr<Pipeline::CPipelineBase> computePipeline = Pipeline::CPipelineBase::Builder().
    addShaderStage("shaders/generators/irradiancecube.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    computePipeline->bind(commandBuffer);

    descriptor.create(computePipeline);
    descriptor.set("outColour", irradianceCubemap->getDescriptor());
    descriptor.set("samplerColour", source->getDescriptor());
    descriptor.update(0);
    descriptor.bind(commandBuffer, 0);

    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    Loaders::CImageLoader::close(&offscreen);

    return irradianceCubemap;
}

std::shared_ptr<CImage> CPBRCompositionPass::ComputePrefiltered(const std::shared_ptr<CImage> &source, uint32_t size)
{
    auto prefilteredCubemap = std::make_shared<CImage>();

    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::CImageLoader::allocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR16G16B16A16Sfloat), true);
    offscreen->isCubemap = true;
    prefilteredCubemap->initializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    prefilteredCubemap->transitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    prefilteredCubemap->updateDescriptor();

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto descriptor = Descriptor::CDescriptorHandler();
    auto push = CPushHandler();
    
    std::shared_ptr<Pipeline::CPipelineBase> computePipeline = Pipeline::CPipelineBase::Builder().
    addShaderStage("shaders/generators/prefilteredmap.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    push.create(*computePipeline->getShader()->getPushBlock("object"));

    for (uint32_t i = 0; i < prefilteredCubemap->getMipLevels(); i++)
    {
        vk::ImageView levelView = VK_NULL_HANDLE;
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::eCube;
        viewInfo.format = prefilteredCubemap->getFormat();
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = i;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 6;
		levelView = CImage::createImageView(prefilteredCubemap->getImage(), viewInfo);

        cmdBuf.begin();
        auto commandBuffer = cmdBuf.getCommandBuffer();
        computePipeline->bind(commandBuffer);
        auto outColor = computePipeline->getShader()->getUniform("outColour");

        auto imageInfo = prefilteredCubemap->getDescriptor();
		imageInfo.imageView = levelView;

        vk::WriteDescriptorSet descriptorWrite = {};
		descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
		descriptorWrite.dstBinding = outColor->getBinding();
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = outColor->getDescriptorType();
		descriptorWrite.pImageInfo = &imageInfo;

        push.set("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->getMipLevels() - 1), 0);

        descriptor.create(computePipeline);
        descriptor.set("outColour", descriptorWrite);
        descriptor.set("samplerColour", source->getDescriptor());
        descriptor.update(0);
        descriptor.bind(commandBuffer, USwapChain->getCurrentFrame());
        push.flush(commandBuffer, computePipeline);

        auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
        auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
        commandBuffer.dispatch(groupCountX, groupCountY, 1);
        cmdBuf.submitIdle();

        UDevice->destroy(levelView);
    }

    Loaders::CImageLoader::close(&offscreen);
    return prefilteredCubemap;
}