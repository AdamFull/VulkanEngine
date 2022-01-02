#include "GeneratorBase.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Loaders;
using namespace Engine::Core::Pipeline;
using namespace Engine::Resources::Material::Generator;

GeneratorBase::~GeneratorBase()
{
	UDevice->Destroy(framebuffer);
    UDevice->Destroy(renderPass);
}

void GeneratorBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    MaterialBase::Create(pResMgr);
}

void GeneratorBase::Cleanup()
{
    MaterialBase::Cleanup();
}

void GeneratorBase::Generate(std::shared_ptr<Mesh::MeshBase> pMesh)
{

}

std::shared_ptr<Image> GeneratorBase::Get()
{
    return m_pGeneratedImage;
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
	attDesc.finalLayout = finalLayout;

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

    renderPass = UDevice->GetLogical().createRenderPass(renderPassCI);
}

void GeneratorBase::CreateFramebuffer()
{
    vk::FramebufferCreateInfo framebufferCI = {};
	framebufferCI.renderPass = renderPass;
	framebufferCI.attachmentCount = 1;
	framebufferCI.pAttachments = &m_pGeneratedImage->GetDescriptor().imageView;
	framebufferCI.width = m_pGeneratedImage->GetExtent().width;
	framebufferCI.height = m_pGeneratedImage->GetExtent().height;
	framebufferCI.layers = 1;

	framebuffer = UDevice->GetLogical().createFramebuffer(framebufferCI);
}

void GeneratorBase::CreateTextures()
{
	m_pGeneratedImage = std::make_shared<Image>();
    ktxTexture *offscreen;
    ImageLoader::AllocateRawDataAsKTXTexture(&offscreen, &imageFormat, m_iDimension, m_iDimension, 1, 2, iFormat);
    m_pGeneratedImage->InitializeTexture(offscreen, imageFormat, usageFlags);
    m_pGeneratedImage->UpdateDescriptor();

	if(bTransfer)
    	m_pGeneratedImage->TransitionImageLayout(vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor);

    ImageLoader::Close(&offscreen);
}