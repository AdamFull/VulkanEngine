#include "PBRCompositionPass.h"
#include "resources/materials/MaterialDeferred.h"
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
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Core::Scene::Objects::Components::Light;


void CPBRCompositionPass::create(std::unique_ptr<FRenderCreateInfo>& createInfo)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));

    m_pSkybox = createInfo->resourceManager->Get<Image>("skybox_cubemap_tex");

    brdf = UHLInstance->GetThreadPool()->submit(&CPBRCompositionPass::ComputeBRDFLUT, 512);
    irradiance = UHLInstance->GetThreadPool()->submit(&CPBRCompositionPass::ComputeIrradiance, m_pSkybox, 64);
    prefiltered = UHLInstance->GetThreadPool()->submit(&CPBRCompositionPass::ComputePrefiltered, m_pSkybox, 512);

    m_pMaterial = std::make_shared<MaterialDeferred>();
    m_pMaterial->Create(createInfo->renderPass, createInfo->subpass);
    CSubpass::create(createInfo);
}

void CPBRCompositionPass::render(std::unique_ptr<FRenderProcessInfo>& renderData)
{
    m_pMaterial->AddTexture("brdflut_tex", *brdf);
    m_pMaterial->AddTexture("irradiance_tex", *irradiance);
    m_pMaterial->AddTexture("prefiltred_tex", *prefiltered);
    m_pMaterial->AddTexture("position_tex", renderData->images["position_tex"]);
    m_pMaterial->AddTexture("lightning_mask_tex", renderData->images["lightning_mask_tex"]);
    m_pMaterial->AddTexture("normal_tex", renderData->images["normal_tex"]);
    m_pMaterial->AddTexture("albedo_tex", renderData->images["albedo_tex"]);
    m_pMaterial->AddTexture("emission_tex", renderData->images["emission_tex"]);
    m_pMaterial->AddTexture("mrah_tex", renderData->images["mrah_tex"]);

    auto imageIndex = USwapChain->GetCurrentFrame();

    //May be move to CompositionObject
    FLightningData ubo;
    auto camera = CameraManager::getInstance()->GetCurrentCamera();
    auto vLights = LightSourceManager::getInstance()->GetSources();
    for(std::size_t i = 0; i < vLights.size(); i++)
        ubo.lights[i] = vLights.at(i);

    ubo.lightCount = vLights.size();
    ubo.viewPos = camera->viewPos; //camera->viewPos
    ubo.bloomThreshold = GlobalVariables::bloomThreshold;
    
    m_pUniform->UpdateUniformBuffer(imageIndex, &ubo);
    auto& buffer = m_pUniform->GetUniformBuffer(imageIndex);
    auto descriptor = buffer->GetDscriptor();
    m_pMaterial->Update(descriptor, imageIndex);
    m_pMaterial->Bind(renderData->commandBuffer, imageIndex);

    renderData->commandBuffer.draw(3, 1, 0, 0);
}

void CPBRCompositionPass::cleanup()
{
    CSubpass::cleanup();
}

std::shared_ptr<Image> CPBRCompositionPass::ComputeBRDFLUT(uint32_t size)
{
    auto brdfImage = std::make_shared<Image>();
    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR16G16Sfloat));
    brdfImage->InitializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    brdfImage->TransitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    brdfImage->UpdateDescriptor();

    auto cmdBuf = CommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.GetCommandBuffer();
    auto descriptor = Descriptor::DescriptorHandler();

    std::shared_ptr<Pipeline::PipelineBase> computePipeline = Pipeline::PipelineBase::Builder().
    addShaderStage("../../assets/shaders/generators/brdflut.comp").
    build();

    computePipeline->Bind(commandBuffer);

    descriptor.Create(computePipeline);
    descriptor.Set("outColour", brdfImage->GetDescriptor());
    descriptor.Update(0);
    descriptor.Bind(commandBuffer, 0);
    
    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    Loaders::ImageLoader::Close(&offscreen);

    return brdfImage;
}

std::shared_ptr<Image> CPBRCompositionPass::ComputeIrradiance(const std::shared_ptr<Image> &source, uint32_t size)
{
    auto irradianceCubemap = std::make_shared<Image>();

    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR32G32B32A32Sfloat));
    offscreen->isCubemap = true;
    irradianceCubemap->InitializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    irradianceCubemap->TransitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    irradianceCubemap->UpdateDescriptor();

    auto cmdBuf = CommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto commandBuffer = cmdBuf.GetCommandBuffer();
    auto descriptor = Descriptor::DescriptorHandler();
    
    std::shared_ptr<Pipeline::PipelineBase> computePipeline = Pipeline::PipelineBase::Builder().
    addShaderStage("../../assets/shaders/generators/irradiancecube.comp").
    build();

    computePipeline->Bind(commandBuffer);

    descriptor.Create(computePipeline);
    descriptor.Set("outColour", irradianceCubemap->GetDescriptor());
    descriptor.Set("samplerColour", source->GetDescriptor());
    descriptor.Update(0);
    descriptor.Bind(commandBuffer, 0);

    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();

    Loaders::ImageLoader::Close(&offscreen);

    return irradianceCubemap;
}

std::shared_ptr<Image> CPBRCompositionPass::ComputePrefiltered(const std::shared_ptr<Image> &source, uint32_t size)
{
    auto prefilteredCubemap = std::make_shared<Image>();

    ktxTexture *offscreen;
    vk::Format format{};
    Loaders::ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &format, size, size, 1, 2, VulkanStaticHelper::VkFormatToGLFormat(vk::Format::eR16G16B16A16Sfloat), true);
    offscreen->isCubemap = true;
    prefilteredCubemap->InitializeTexture(offscreen, format, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage);
    prefilteredCubemap->TransitionImageLayout(vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    prefilteredCubemap->UpdateDescriptor();

    auto cmdBuf = CommandBuffer(true, vk::QueueFlagBits::eCompute);
    auto descriptor = Descriptor::DescriptorHandler();
    auto push = PushHandler();
    
    std::shared_ptr<Pipeline::PipelineBase> computePipeline = Pipeline::PipelineBase::Builder().
    addShaderStage("../../assets/shaders/generators/prefilteredmap.comp").
    build();
    push.Create(*computePipeline->GetShader()->GetUniformBlock("PushObject"));

    for (uint32_t i = 0; i < prefilteredCubemap->GetMipLevels(); i++)
    {
        vk::ImageView levelView = VK_NULL_HANDLE;
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::eCube;
        viewInfo.format = prefilteredCubemap->GetFormat();
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = i;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 6;
		levelView = Image::CreateImageView(prefilteredCubemap->GetImage(), viewInfo);

        cmdBuf.begin();
        auto commandBuffer = cmdBuf.GetCommandBuffer();
        computePipeline->Bind(commandBuffer);

        auto imageInfo = prefilteredCubemap->GetDescriptor();
		imageInfo.imageView = levelView;

        vk::WriteDescriptorSet descriptorWrite = {};
		descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
		descriptorWrite.dstBinding = *computePipeline->GetShader()->GetDescriptorLocation("outColour");
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = *computePipeline->GetShader()->GetDescriptorType(descriptorWrite.dstBinding);
		descriptorWrite.pImageInfo = &imageInfo;

        push.Set("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->GetMipLevels() - 1), 0);

        descriptor.Create(computePipeline);
        descriptor.Set("outColour", descriptorWrite);
        descriptor.Set("samplerColour", source->GetDescriptor());
        descriptor.Update(0);
        descriptor.Bind(commandBuffer, USwapChain->GetCurrentFrame());
        push.Flush(commandBuffer, computePipeline);

        auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[0])));
        auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*computePipeline->GetShader()->GetLocalSizes()[1])));
        commandBuffer.dispatch(groupCountX, groupCountY, 1);
        cmdBuf.submitIdle();

        UDevice->Destroy(levelView);
    }

    Loaders::ImageLoader::Close(&offscreen);
    return prefilteredCubemap;
}