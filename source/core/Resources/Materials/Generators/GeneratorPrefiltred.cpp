#include "GeneratorPrefiltred.h"
#include "Core/VulkanUniform.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

GeneratorPrefiltred::GeneratorPrefiltred(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain)
{
    m_device = device;
    m_swapchain = swapchain;
}

void GeneratorPrefiltred::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    m_iDimension = 512;
    CreateTextures();

    vk::PushConstantRange constantRange{};
    constantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    constantRange.size = sizeof(FPrefiltredPushBlock);
    m_vConstantRanges.emplace_back(constantRange);

    GeneratorBase::Create(pResMgr);
}

void GeneratorPrefiltred::CreateDescriptors(uint32_t images)
{
    GeneratorBase::CreateDescriptors(images);

    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build();

    auto texSet = Core::FDefaultAllocator::Allocate<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);
    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &m_mTextures[ETextureAttachmentType::eCubemap]->GetDescriptor()).
    Build();
    m_pMatDesc->Update(0, 0, imageInfo);
}

void GeneratorPrefiltred::Generate(std::shared_ptr<Mesh::MeshBase> pMesh)
{
    GeneratorBase::Generate(pMesh);

    vk::ClearValue clearValues[1];
	clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.2f, 0.0f}};

	vk::RenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.extent.width = m_iDimension;
	renderPassBeginInfo.renderArea.extent.height = m_iDimension;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = framebuffer;

    vk::CommandBuffer tempBuffer = m_device->BeginSingleTimeCommands();

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

    m_pCubemap->TransitionImageLayout(tempBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor);

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
            FUniformData ubo{};
            pMesh->Render(tempBuffer, 0, ubo);
            tempBuffer.endRenderPass();

            m_pGeneratedImage->TransitionImageLayout(tempBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor);

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

            m_pGeneratedImage->CopyImageToDst(tempBuffer, m_pCubemap, copyRegion, vk::ImageLayout::eTransferDstOptimal);
            m_pGeneratedImage->TransitionImageLayout(tempBuffer, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor);
        }
    }

    m_pCubemap->TransitionImageLayout(tempBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor);
    m_pCubemap->UpdateDescriptor();
    
    m_device->EndSingleTimeCommands(tempBuffer);
}

std::shared_ptr<TextureBase> GeneratorPrefiltred::Get()
{
    return m_pCubemap;
}

void GeneratorPrefiltred::CreateTextures()
{
    GeneratorBase::CreateTextures();

    m_pCubemap = Core::FDefaultAllocator::Allocate<TextureBase>();
    ktxTexture *cubemap;
    ImageLoader::AllocateRawDataAsKTXTextureCubemap(&cubemap, &imageFormat, m_iDimension, m_iDimension, 1, 2, 0x881A, true);
    m_pCubemap->InitializeTexture(cubemap, imageFormat, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
    m_pCubemap->UpdateDescriptor();
    ImageLoader::Close(&cubemap);

    GeneratorBase::CreateTextures();
}