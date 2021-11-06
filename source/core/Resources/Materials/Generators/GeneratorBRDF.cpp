#include "GeneratorBRDF.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core::Descriptor;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

void GeneratorBRDF::Create()
{
    GeneratorBase::Create();
}

void GeneratorBRDF::CreateDescriptors(uint32_t images)
{
    GeneratorBase::CreateDescriptors(images);

    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build(UDevice);

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(UDevice, m_pDescriptorPool, texSetLayout, images);
    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}

void GeneratorBRDF::Generate(uint32_t indexCount, uint32_t firstIndex)
{
    GeneratorBase::Generate(indexCount, firstIndex);

    vk::ClearValue clearValues[1];
	clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};

	vk::RenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.extent.width = m_iDimension;
	renderPassBeginInfo.renderArea.extent.height = m_iDimension;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = framebuffer;

    vk::CommandBuffer tempBuffer = UDevice->BeginSingleTimeCommands();
    tempBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport.width = (float)m_iDimension;
    viewport.height = (float)m_iDimension;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
	vk::Rect2D scissor{};
    scissor.extent.width = m_iDimension;
    scissor.extent.height = m_iDimension;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    tempBuffer.setViewport(0, 1, &viewport);
    tempBuffer.setScissor(0, 1, &scissor);
    tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline->GetPipeline());
    tempBuffer.draw(3, 1, 0, 0);
    tempBuffer.endRenderPass();
    UDevice->EndSingleTimeCommands(tempBuffer);
}

void GeneratorBRDF::CreateTextures()
{
    m_pGeneratedImage = std::make_shared<TextureBase>();

    ktxTexture *texture;
    ImageLoader::AllocateRawDataAsKTXTexture(&texture, &imageFormat, m_iDimension, m_iDimension, 1, 2, 0x822F);

    m_pGeneratedImage->InitializeTexture(texture, imageFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    m_pGeneratedImage->UpdateDescriptor();

    ImageLoader::Close(&texture);
    
    GeneratorBase::CreateTextures();
}