#include "GeneratorBase.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Textures/ImageLoader.h"
#include "Core/Pipeline/VulkanPipeline.h"
#include "Core/Pipeline/PipelineFactory.h"
#include "Core/Descriptor/DescriptorPool.h"
#include "Core/Descriptor/DescriptorSetContainer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core::Pipeline;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Loaders;
using namespace Engine::Core::Descriptor;

std::shared_ptr<TextureBase> GeneratorBase::Generate(EGeneratorType eType, uint32_t dimension)
{
    auto pTexture = std::make_shared<TextureBase>();
    vk::Format format;
    ktxTexture *texture;
    ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, dimension, dimension, 1, 2, 0x822F);

    pTexture->InitializeTexture(texture, format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    pTexture->UpdateDescriptor();

    ImageLoader::Close(&texture);

    auto pTemporary = std::make_shared<FGeneratorTemporary>();
    pTemporary->renderPass = CreateRenderPass(format);
    pTemporary->framebuffer = CreateFramebuffer(pTexture, pTemporary);
    pTemporary->m_pDescriptorPool = VulkanDescriptorPool::Builder().setMaxSets(2).addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1).build(UDevice);
    pTemporary->m_pMatDesc = std::make_shared<VulkanDescriptorSetContainer>();

    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build(UDevice);

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(UDevice, pTemporary->m_pDescriptorPool, texSetLayout, 2);
    pTemporary->m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));

    pTemporary->pipelineCache = CreatePipelineCache(pTemporary);
    pTemporary->pipelineLayout = CreatePipelineLayout(pTemporary);

    auto pPipeline = CreatePipeline(pTemporary, EShaderSet::eBRDF);

    vk::ClearValue clearValues[1];
	clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};

	vk::RenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.renderPass = pTemporary->renderPass;
	renderPassBeginInfo.renderArea.extent.width = dimension;
	renderPassBeginInfo.renderArea.extent.height = dimension;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = pTemporary->framebuffer;

    vk::CommandBuffer tempBuffer = UDevice->BeginSingleTimeCommands();
    tempBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport{};
    viewport.width = (float)dimension;
    viewport.height = (float)dimension;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
	vk::Rect2D scissor{};
    scissor.extent.width = dimension;
    scissor.extent.height = dimension;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    tempBuffer.setViewport(0, 1, &viewport);
    tempBuffer.setScissor(0, 1, &scissor);
    tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline->GetPipeline());
    tempBuffer.draw(3, 1, 0, 0);
    tempBuffer.endRenderPass();
    UDevice->EndSingleTimeCommands(tempBuffer);
    
    return pTexture;
}

vk::RenderPass GeneratorBase::CreateRenderPass(vk::Format format)
{
    vk::AttachmentDescription attDesc = {};
	// Color attachment
	attDesc.format = format;
	attDesc.samples = vk::SampleCountFlagBits::e1;
	attDesc.loadOp = vk::AttachmentLoadOp::eClear;
	attDesc.storeOp = vk::AttachmentStoreOp::eStore;
	attDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attDesc.initialLayout = vk::ImageLayout::eUndefined;
	attDesc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	vk::AttachmentReference colorReference = { 0, vk::ImageLayout::eColorAttachmentOptimal };

	vk::SubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;

	// Use subpass dependencies for layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

	// Create the actual renderpass
	vk::RenderPassCreateInfo renderPassCI = {};
	renderPassCI.attachmentCount = 1;
	renderPassCI.pAttachments = &attDesc;
	renderPassCI.subpassCount = 1;
	renderPassCI.pSubpasses = &subpassDescription;
	renderPassCI.dependencyCount = 2;
	renderPassCI.pDependencies = dependencies.data();

    return UDevice->GetLogical()->createRenderPass(renderPassCI);
}

vk::Framebuffer GeneratorBase::CreateFramebuffer(std::shared_ptr<TextureBase> pTexture, std::shared_ptr<FGeneratorTemporary> pTemp)
{
    vk::FramebufferCreateInfo framebufferCI = {};
	framebufferCI.renderPass = pTemp->renderPass;
	framebufferCI.attachmentCount = 1;
	framebufferCI.pAttachments = &pTexture->GetDescriptor().imageView;
	framebufferCI.width = pTexture->GetParams().width;
	framebufferCI.height = pTexture->GetParams().height;
	framebufferCI.layers = 1;

	return UDevice->GetLogical()->createFramebuffer(framebufferCI);
}

vk::PipelineLayout GeneratorBase::CreatePipelineLayout(std::shared_ptr<FGeneratorTemporary> pTemp)
{
    std::vector<vk::DescriptorSetLayout> vLayouts = pTemp->m_pMatDesc->GetLayouts();
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.setLayoutCount = vLayouts.size();
    pipelineLayoutInfo.pSetLayouts = vLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    return UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
}

vk::PipelineCache GeneratorBase::CreatePipelineCache(std::shared_ptr<FGeneratorTemporary> pTemp)
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    return UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
}

std::shared_ptr<PipelineBase> GeneratorBase::CreatePipeline(std::shared_ptr<FGeneratorTemporary> pTemp, EShaderSet eSet)
{
    FPipelineCreateInfo CI; 

    switch (eSet)
    {
    case EShaderSet::eBRDF:
        CI = PipelineConfig::CreateBRDFPipeline(pTemp->renderPass, vk::SampleCountFlagBits::e1, pTemp->pipelineLayout, pTemp->pipelineCache);
        break;
    }

    return PipelineFactory::CreatePipeline(CI, UDevice, USwapChain);
}
