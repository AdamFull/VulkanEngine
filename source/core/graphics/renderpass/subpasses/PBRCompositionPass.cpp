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
    CRenderSystem::inst()->setStageType(EStageType::eDeferred);
    auto& images = CRenderSystem::inst()->getCurrentImages();
    pMaterial->addTexture("brdflut_tex", *brdf);
    pMaterial->addTexture("irradiance_tex", *irradiance);
    pMaterial->addTexture("prefiltred_tex", *prefiltered);
    
    pMaterial->addTexture("packed_tex", images["packed_tex"]);
    pMaterial->addTexture("emission_tex", images["emission_tex"]);
    pMaterial->addTexture("depth_tex", images["depth_tex"]);
    pMaterial->addTexture("ssr_tex", images["ssr_tex"]);
    
    pMaterial->addTexture("cascade_shadowmap_tex", images["cascade_shadowmap_tex"]);
    pMaterial->addTexture("direct_shadowmap_tex", images["direct_shadowmap_tex"]);
    pMaterial->addTexture("omni_shadowmap_tex", images["omni_shadowmap_tex"]);

    auto imageIndex = CDevice::inst()->getCurrentFrame();

    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto invViewProjection = glm::inverse(projection * view);

    uint32_t directoonal_light_count{0};
    auto aDirectionalLights = CLightSourceManager::inst()->getDirectionalSources(directoonal_light_count);

    uint32_t point_light_count{0};
    auto aPointLights = CLightSourceManager::inst()->getPointSources(point_light_count);

    uint32_t spot_light_count{0};
    auto aSpotLights = CLightSourceManager::inst()->getSpotSources(spot_light_count);
    
    auto& pUBO = pMaterial->getUniformBuffer("UBODeferred");
    pUBO->set("invViewProjection", invViewProjection);
    pUBO->set("view", view);
    pUBO->set("viewPos", camera->viewPos);
    pUBO->set("directionalLightCount", directoonal_light_count);
    pUBO->set("spotLightCount", spot_light_count);
    pUBO->set("pointLightCount", point_light_count);

    auto& pDebugUBO = pMaterial->getUniformBuffer("UBODebug");
    pDebugUBO->set("shading_mode", GlobalVariables::shadingMode);
    pDebugUBO->set("target", GlobalVariables::debugTarget);
    pDebugUBO->set("cascade", GlobalVariables::debugCascade);

    auto& pUBOLights = pMaterial->getUniformBuffer("UBOLights");
    pUBOLights->set("directionalLights", aDirectionalLights);
    pUBOLights->set("spotLights", aSpotLights);
    pUBOLights->set("pointLights", aPointLights);
    
    pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}

void CPBRCompositionPass::cleanup()
{
    CSubpass::cleanup();
}

utl::ref_ptr<CImage> CPBRCompositionPass::ComputeBRDFLUT(uint32_t size)
{
    auto brdfImage = utl::make_ref<CImage2D>(vk::Extent2D{size, size}, vk::Format::eR16G16Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    auto generator = CMaterialLoader::inst()->create("brdflut_generator");
    auto& pipeline = generator->getPipeline();
    generator->create();
    generator->addTexture("outColour", brdfImage->getDescriptor());
    generator->update();
    generator->bind(commandBuffer);
    
    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    return brdfImage;
}

utl::ref_ptr<CImage> CPBRCompositionPass::ComputeIrradiance(const utl::ref_ptr<CImage> &source, uint32_t size)
{
    auto irradianceCubemap = utl::make_ref<CImageCubemap>(vk::Extent2D{size, size}, vk::Format::eR32G32B32A32Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    
    auto generator = CMaterialLoader::inst()->create("irradiancecube_generator");
    auto& pipeline = generator->getPipeline();
    generator->create();
    generator->addTexture("outColour", irradianceCubemap->getDescriptor());
    generator->addTexture("samplerColour", source->getDescriptor());
    generator->update();
    generator->bind(commandBuffer);

    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    return irradianceCubemap;
}

utl::ref_ptr<CImage> CPBRCompositionPass::ComputePrefiltered(const utl::ref_ptr<CImage>& source, uint32_t size)
{
    auto prefilteredCubemap = utl::make_ref<CImageCubemap>(vk::Extent2D{size, size}, vk::Format::eR16G16B16A16Sfloat, vk::ImageLayout::eGeneral,
    vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage,
    vk::ImageAspectFlagBits::eColor, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, true, false, true);

    auto cmdBuf = CCommandBuffer(true, vk::QueueFlagBits::eCompute);
    
    auto generator = CMaterialLoader::inst()->create("prefilteredmap_generator");
    auto& pipeline = generator->getPipeline();
    generator->create(); 
    auto& push = generator->getPushConstant("object");   

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

        auto imageInfo = prefilteredCubemap->getDescriptor();
		imageInfo.imageView = levelView;

        push->set("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->getMipLevels() - 1));
        push->flush(commandBuffer);

        generator->addTexture("outColour", imageInfo);
        generator->addTexture("samplerColour", source->getDescriptor());
        generator->update();
        generator->bind(commandBuffer);

        auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[0])));
        auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pipeline->getShader()->getLocalSizes()[1])));
        commandBuffer.dispatch(groupCountX, groupCountY, 1);
        cmdBuf.submitIdle();

        CDevice::inst()->destroy(&levelView);
    }

    return prefilteredCubemap;
}
