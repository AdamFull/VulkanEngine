#include "GeneratorPrefiltred.h"

#include "Core/Descriptor/DescriptorWriter.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core::Descriptor;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

void GeneratorPrefiltred::Create()
{
    m_iDimension = 512;
    CreateTextures();

    vk::PushConstantRange constantRange{};
    constantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    constantRange.size = sizeof(FPrefiltredPushBlock);
    m_vConstantRanges.emplace_back(constantRange);

    GeneratorBase::Create();
}

void GeneratorPrefiltred::CreateDescriptors(uint32_t images)
{
    GeneratorBase::CreateDescriptors(images);

    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build(UDevice);

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(UDevice, m_pDescriptorPool, texSetLayout, images);
    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &m_mTextures[ETextureAttachmentType::eCubemap]->GetDescriptor()).
    Build();
    m_pMatDesc->Update(0, 0, imageInfo);
}

void GeneratorPrefiltred::Generate(uint32_t indexCount, uint32_t firstIndex)
{
    GeneratorBase::Generate(indexCount, firstIndex);

    vk::ClearValue clearValues[1];
	clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.2f, 0.0f}};

	vk::RenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.extent.width = m_iDimension;
	renderPassBeginInfo.renderArea.extent.height = m_iDimension;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = framebuffer;

    vk::CommandBuffer tempBuffer = UDevice->BeginSingleTimeCommands();

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

    m_pCubemap->TransitionImageLayout(tempBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    FPrefiltredPushBlock pushBlock;

    for (uint32_t m = 0; m < m_pCubemap->GetParams().mipLevels; m++) 
    {
		for (uint32_t f = 0; f < 6; f++) 
        {
            viewport.width = static_cast<float>(m_iDimension * std::pow(0.5f, m));
			viewport.height = static_cast<float>(m_iDimension * std::pow(0.5f, m));
            tempBuffer.setViewport(0, 1, &viewport);

            tempBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            pushBlock.mvp = glm::perspective((float)(3.14 / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
            tempBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(FPrefiltredPushBlock), &pushBlock);
            tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline->GetPipeline());
            m_pMatDesc->Bind(tempBuffer, 0);
            UVBO->Bind(tempBuffer);
            tempBuffer.drawIndexed(indexCount, 1, firstIndex, 0, 0);
            tempBuffer.endRenderPass();

            m_pGeneratedImage->TransitionImageLayout(tempBuffer, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);

            vk::ImageCopy copyRegion = {};

            copyRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            copyRegion.srcSubresource.baseArrayLayer = 0;
            copyRegion.srcSubresource.mipLevel = 0;
            copyRegion.srcSubresource.layerCount = 1;
            copyRegion.srcOffset = vk::Offset3D{ 0, 0, 0 };

            copyRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            copyRegion.dstSubresource.baseArrayLayer = f;
            copyRegion.dstSubresource.mipLevel = m;
            copyRegion.dstSubresource.layerCount = 1;
            copyRegion.dstOffset = vk::Offset3D{ 0, 0, 0 };

            copyRegion.extent.width = static_cast<uint32_t>(viewport.width);
            copyRegion.extent.height = static_cast<uint32_t>(viewport.height);
	        copyRegion.extent.depth = 1;

            m_pGeneratedImage->CopyImageToDst(tempBuffer, m_pCubemap, copyRegion, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eTransferDstOptimal);
            m_pGeneratedImage->TransitionImageLayout(tempBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eColorAttachmentOptimal);
        }
    }

    m_pCubemap->TransitionImageLayout(tempBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    
    UDevice->EndSingleTimeCommands(tempBuffer);
}

std::shared_ptr<TextureBase> GeneratorPrefiltred::Get()
{
    return m_pCubemap;
}

void GeneratorPrefiltred::CreateTextures()
{
    m_pGeneratedImage = std::make_shared<TextureBase>();
    ktxTexture *offscreen;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, m_iDimension, m_iDimension, 1, 2, 0x881A);
    m_pGeneratedImage->InitializeTexture(offscreen, imageFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    m_pGeneratedImage->UpdateDescriptor();
    ImageLoader::Close(&offscreen);

    m_pCubemap = std::make_shared<TextureBase>();
    ktxTexture *cubemap;
    ImageLoader::AllocateRawDataAsKTXTextureCubemap(&cubemap, &imageFormat, m_iDimension, m_iDimension, 1, 2, 0x881A, true);
    m_pCubemap->InitializeTexture(cubemap, imageFormat, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
    m_pCubemap->UpdateDescriptor();
    ImageLoader::Close(&cubemap);

    GeneratorBase::CreateTextures();
}