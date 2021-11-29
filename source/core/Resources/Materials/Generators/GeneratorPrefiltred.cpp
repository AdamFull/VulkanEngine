#include "GeneratorPrefiltred.h"
#include "Core/VulkanUniform.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"
#include "Resources/Meshes/Primitives.hpp"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;

void GeneratorPrefiltred::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    m_iDimension = 512;
    //CreateTextures();

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

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);
    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &m_mTextures[ETextureAttachmentType::eCubemap]).
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

    vk::CommandBuffer tempBuffer = UDevice->BeginSingleTimeCommands();

    vk::Viewport viewport = Core::Initializers::Viewport(m_iDimension, m_iDimension);
	vk::Rect2D scissor = Core::Initializers::Scissor(m_iDimension, m_iDimension);

    tempBuffer.setViewport(0, 1, &viewport);
    tempBuffer.setScissor(0, 1, &scissor);

    m_pCubemap->TransitionImageLayout(tempBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor);

    FPrefiltredPushBlock pushBlock;

    auto pLocalVBO = std::make_unique<VulkanVBO>();
    auto vertices = FCube::m_vVertices;
    auto indices = FCube::m_vIndices;
    pLocalVBO->AddMeshData(std::move(vertices), std::move(indices));
    pLocalVBO->Create();
    pLocalVBO->Bind(tempBuffer);

    for (uint32_t m = 0; m < m_pCubemap->GetParams().mipLevels; m++) 
    {
        pushBlock.roughness = (float)m / (float)(m_pCubemap->GetParams().mipLevels - 1);
		for (uint32_t f = 0; f < 6; f++) 
        {
            viewport.width = static_cast<float>(m_iDimension * std::pow(0.5f, m));
			viewport.height = static_cast<float>(m_iDimension * std::pow(0.5f, m));
            tempBuffer.setViewport(0, 1, &viewport);

            tempBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            pushBlock.mvp = glm::perspective((float)(LONGPI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
            tempBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(FPrefiltredPushBlock), &pushBlock);
            tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline->GetPipeline());
            m_pMatDesc->Bind(tempBuffer, 0);

            tempBuffer.drawIndexed(pLocalVBO->GetLastIndex(), 1, 0, 0, 0);
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
    
    UDevice->EndSingleTimeCommands(tempBuffer);
}

std::shared_ptr<Image> GeneratorPrefiltred::Get()
{
    return m_pCubemap;
}

void GeneratorPrefiltred::CreateTextures()
{
    GeneratorBase::CreateTextures();

    m_pCubemap = std::make_shared<Image>();
    ktxTexture *cubemap;
    ImageLoader::AllocateRawDataAsKTXTextureCubemap(&cubemap, &imageFormat, m_iDimension, m_iDimension, 1, 2, 0x881A, true);
    cubemap->isCubemap = true;
    m_pCubemap->InitializeTexture(cubemap, imageFormat, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
    m_pCubemap->UpdateDescriptor();
    ImageLoader::Close(&cubemap);
}