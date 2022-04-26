#include "PBRCompositionRenderer.h"
#include "graphics/image/Image.h"
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
#include "GlobalVariables.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects;
using namespace Engine::Core::Scene::Objects::Components;
using namespace Engine::Core::Scene::Objects::Components::Light;

void PBRCompositionRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{
    auto framesInFlight = USwapChain->GetFramesInFlight();
    m_pUniform = std::make_shared<UniformBuffer>();
    m_pUniform->Create(framesInFlight, sizeof(FLightningData));

    m_eRendererType = renderer_type_t::ePBRComposition;

    m_vColorAttachments = 
    {
        {
            ETextureAttachmentType::eDiffuseAlbedo,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        },
        {
            ETextureAttachmentType::eBrightness,
            FRendererCreateInfo::FAttachmentInfo
            (
                vk::Format::eR16G16B16A16Sfloat,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                {std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}
            )
        }
    };

    out_extent = USwapChain->GetExtent();
    RendererBase::Create(pResMgr);

    brdf = UHLInstance->GetThreadPool()->submit(&PBRCompositionRenderer::ComputeBRDFLUT, 512);

    CreateMaterial(pResMgr);
}

void PBRCompositionRenderer::ReCreate(uint32_t framesInFlight)
{
    out_extent = USwapChain->GetExtent();
    RendererBase::ReCreate(framesInFlight);
    m_pMaterial->ReCreate();
}

void PBRCompositionRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    m_pMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, *brdf);

    BeginRender(commandBuffer);
    auto imageIndex = USwapChain->GetCurrentFrame();
    const auto& mImages = m_pPrev->GetProducts();
    for(auto& [attachment, texture] : mImages)
        m_pMaterial->AddTexture(attachment, texture);

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
    m_pMaterial->Bind(commandBuffer, imageIndex);

    commandBuffer.draw(3, 1, 0, 0);

    EndRender(commandBuffer);

    RendererBase::Render(commandBuffer);
}

void PBRCompositionRenderer::Cleanup()
{
    RendererBase::Cleanup();
}

void PBRCompositionRenderer::CreateMaterial(std::shared_ptr<ResourceManager> pResMgr)
{
    m_pMaterial = std::make_shared<MaterialDeferred>();
    m_pMaterial->Create(nullptr);
    m_pMaterial->AddTexture(ETextureAttachmentType::eBRDFLUT, *brdf); //pResMgr->Get<Image>("environment_component_brdf")
    m_pMaterial->AddTexture(ETextureAttachmentType::eIrradiance, pResMgr->Get<Image>("environment_component_irradiate_cube"));
    m_pMaterial->AddTexture(ETextureAttachmentType::ePrefiltred, pResMgr->Get<Image>("environment_component_prefiltred_cube"));
}

std::shared_ptr<Image> PBRCompositionRenderer::ComputeBRDFLUT(uint32_t size)
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
    AddShaderStage("../../assets/shaders/generators/brdflut.comp").
    Build(vk::PipelineBindPoint::eCompute);

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

std::shared_ptr<Image> PBRCompositionRenderer::ComputeIrradiance(const std::shared_ptr<Image> &source, uint32_t size)
{
    auto irradianceCubemap = std::make_shared<Image>();
    //glm::vec2(size), vk::Format::eR32G32B32A32Sfloat, vk::ImageLayout::eGeneral

    return irradianceCubemap;
}

std::shared_ptr<Image> PBRCompositionRenderer::ComputePrefiltered(const std::shared_ptr<Image> &source, uint32_t size)
{
    auto prefilteredCubemap = std::make_shared<Image>();
    //glm::vec2(size), vk::Format::eR16G16B16A16Sfloat, vk::ImageLayout::eGeneral, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, true, true

    return prefilteredCubemap;
}
