#include "GeneratorBase.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core::Pipeline;
using namespace Engine::Resources::Texture;

GeneratorBase::~GeneratorBase()
{
	m_device->Destroy(framebuffer);
    m_device->Destroy(renderPass);
}

FPipelineCreateInfo GeneratorBase::CreateInfo(EShaderSet eSet)
{
    switch (eSet)
    {
    case EShaderSet::eBRDF:
        return PipelineConfig::CreateBRDFPipeline(renderPass, vk::SampleCountFlagBits::e1, pipelineLayout, pipelineCache);
        break;
    case EShaderSet::eIrradiateCube:
        return PipelineConfig::CreateIrradiatePipeline(renderPass, vk::SampleCountFlagBits::e1, pipelineLayout, pipelineCache);
        break;
    case EShaderSet::ePrefiltred:
        return PipelineConfig::CreatePrefiltredPipeline(renderPass, vk::SampleCountFlagBits::e1, pipelineLayout, pipelineCache);
        break;
    }

    return MaterialBase::CreateInfo(eSet);
}

void GeneratorBase::Create()
{
    CreateTextures();
    CreateRenderPass(imageFormat);
    CreateFramebuffer();

    MaterialBase::Create();
}

void GeneratorBase::Cleanup()
{
    MaterialBase::Cleanup();
}

void GeneratorBase::Generate(uint32_t indexCount, uint32_t firstIndex)
{

}

std::shared_ptr<TextureBase> GeneratorBase::Get()
{
    return m_pGeneratedImage;
}

void GeneratorBase::CreateDescriptors(uint32_t images)
{
    MaterialBase::CreateDescriptors(images);
}

void GeneratorBase::CreateRenderPass(vk::Format format)
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

    renderPass = m_device->GetLogical().createRenderPass(renderPassCI);
}

void GeneratorBase::CreateFramebuffer()
{
    vk::FramebufferCreateInfo framebufferCI = {};
	framebufferCI.renderPass = renderPass;
	framebufferCI.attachmentCount = 1;
	framebufferCI.pAttachments = &m_pGeneratedImage->GetDescriptor().imageView;
	framebufferCI.width = m_pGeneratedImage->GetParams().width;
	framebufferCI.height = m_pGeneratedImage->GetParams().height;
	framebufferCI.layers = 1;

	framebuffer = m_device->GetLogical().createFramebuffer(framebufferCI);
}

void GeneratorBase::CreateTextures()
{

}