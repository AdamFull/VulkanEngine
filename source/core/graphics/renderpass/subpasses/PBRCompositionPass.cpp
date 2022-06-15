#include "PBRCompositionPass.h"
#include "resources/materials/MaterialLoader.h"
#include "resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "graphics/scene/objects/components/LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanInitializers.h"
#include "graphics/pipeline/Pipeline.h"
#include "graphics/pipeline/ComputePipeline.h"
#include "graphics/descriptor/DescriptorHandler.h"
#include "graphics/image/Image.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageCubemap.h"
#include "graphics/buffer/PushHandler.hpp"
#include "GlobalVariables.h"

using namespace engine::core;
using namespace engine::core::descriptor;
using namespace engine::core::pipeline;
using namespace engine::core::render;
using namespace engine::resources;
using namespace engine::resources::material;
using namespace engine::core::scene;


void CPBRCompositionPass::create()
{
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    m_pSkybox = CResourceManager::inst()->get<CImage>("skybox_cubemap_tex");

    brdf = CThreadPool::inst()->submit(&CPBRCompositionPass::ComputeBRDFLUT, 512);
    irradiance = CThreadPool::inst()->submit(&CPBRCompositionPass::ComputeIrradiance, m_pSkybox, 64);
    prefiltered = CThreadPool::inst()->submit(&CPBRCompositionPass::ComputePrefiltered, m_pSkybox, 512);

    pMaterial = CMaterialLoader::inst()->create("pbr_composition");
    pMaterial->create();
    CSubpass::create();
}

void CPBRCompositionPass::reCreate()
{
    pMaterial->reCreate();
}

void CPBRCompositionPass::render(vk::CommandBuffer& commandBuffer)
{
    auto& images = CRenderSystem::inst()->getCurrentImages();
    pMaterial->addTexture("brdflut_tex", *brdf);
    pMaterial->addTexture("irradiance_tex", *irradiance);
    pMaterial->addTexture("prefiltred_tex", *prefiltered);
    
    pMaterial->addTexture("packed_tex", images["packed_tex"]);
    pMaterial->addTexture("emission_tex", images["emission_tex"]);
    pMaterial->addTexture("depth_tex", images["depth_image"]);

    auto imageIndex = CDevice::inst()->getCurrentFrame();

    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto invViewProjection = glm::inverse(projection * view);
    
    uint32_t light_count{0};
    auto aLights = CLightSourceManager::inst()->getSources(light_count);
    
    auto& pUBO = pMaterial->getPushConstant("ubo");
    pUBO->set("invViewProjection", invViewProjection);
    pUBO->set("viewPos", camera->viewPos);
    pUBO->set("lightCount", light_count);
    pUBO->flush(commandBuffer);

    auto& pUBOLights = pMaterial->getUniformBuffer("UBOLights");
    pUBOLights->set("lights", aLights);
    
    pMaterial->update();
    pMaterial->bind();

    commandBuffer.draw(3, 1, 0, 0);
}

void CPBRCompositionPass::cleanup()
{
    CSubpass::cleanup();
}

ref_ptr<CImage> CPBRCompositionPass::ComputeBRDFLUT(uint32_t size)
{
    auto brdfImage = make_ref<CImage2D>();
    brdfImage->create(vk::Extent2D{size, size}, vk::Format::eR16G16Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    auto descriptor = CDescriptorHandler();

    ref_ptr<CPipelineBase> computePipeline = CPipelineBase::Builder().
    addShaderStage("shaders/generators/brdflut.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    computePipeline->bind(commandBuffer);

    descriptor.create(computePipeline);
    descriptor.set("outColour", brdfImage->getDescriptor());
    descriptor.update();
    descriptor.bind(commandBuffer);
    
    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    return brdfImage;
}

ref_ptr<CImage> CPBRCompositionPass::ComputeIrradiance(const ref_ptr<CImage> &source, uint32_t size)
{
    auto irradianceCubemap = make_ref<CImageCubemap>();
    irradianceCubemap->create(vk::Extent2D{size, size}, vk::Format::eR32G32B32A32Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    auto descriptor = CDescriptorHandler();
    
    ref_ptr<CPipelineBase> computePipeline = CPipelineBase::Builder().
    addShaderStage("shaders/generators/irradiancecube.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    computePipeline->bind(commandBuffer);

    descriptor.create(computePipeline);
    descriptor.set("outColour", irradianceCubemap->getDescriptor());
    descriptor.set("samplerColour", source->getDescriptor());
    descriptor.update();
    descriptor.bind(commandBuffer);

    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    return irradianceCubemap;
}

ref_ptr<CImage> CPBRCompositionPass::ComputePrefiltered(const ref_ptr<CImage>& source, uint32_t size)
{
    auto prefilteredCubemap = make_ref<CImageCubemap>();
    prefilteredCubemap->create(vk::Extent2D{size, size}, vk::Format::eR16G16B16A16Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage,
    vk::ImageAspectFlagBits::eColor, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, true, false, true);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto descriptor = CDescriptorHandler();
    auto push = CPushHandler();
    
    ref_ptr<CPipelineBase> computePipeline = CPipelineBase::Builder().
    addShaderStage("shaders/generators/prefilteredmap.comp").
    build(vk::PipelineBindPoint::eCompute);
    computePipeline->create();
    push.create(*computePipeline->getShader()->getPushBlock("object"), computePipeline);

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
        viewInfo.image = prefilteredCubemap->getImage();
		vk::Result res = CDevice::inst()->create(viewInfo, &levelView);
        assert(res == vk::Result::eSuccess && "Cannot create image view.");

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

        push.set("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->getMipLevels() - 1));

        descriptor.create(computePipeline);
        descriptor.set("outColour", descriptorWrite);
        descriptor.set("samplerColour", source->getDescriptor());
        descriptor.update();
        descriptor.bind(commandBuffer);
        push.flush(commandBuffer);

        auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[0])));
        auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->getShader()->getLocalSizes()[1])));
        commandBuffer.dispatch(groupCountX, groupCountY, 1);
        cmdBuf.submitIdle();

        CDevice::inst()->destroy(&levelView);
    }

    return prefilteredCubemap;
}
