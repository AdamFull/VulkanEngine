#include "IrradianceGenerator.h"
#include "Renderer/VulkanHighLevel.h"
#include "Renderer/VulkanDevice.h"
#include "Resources/Textures/VulkanTexture.h"

namespace Engine
{
    void IrradianceGenerator::Create()
    {
        GeneratorBase::Create();
    }

    void IrradianceGenerator::ReCreate()
    {
        GeneratorBase::ReCreate();
    }

    void IrradianceGenerator::Update(uint32_t imageIndex)
    {
        GeneratorBase::Update(imageIndex, pUniformBuffer);
    }

    void IrradianceGenerator::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        GeneratorBase::Bind(commandBuffer, imageIndex);
    }

    void IrradianceGenerator::Cleanup()
    {
        GeneratorBase::Cleanup();
    }

    void IrradianceGenerator::Destroy()
    {
        GeneratorBase::Destroy();
    }

    void IrradianceGenerator::CreateRenderPass()
    {
        GeneratorBase::CreateRenderPass();
        //It's for irradiance
        vk::AttachmentDescription attachmentDesc = {};
		// Color attachment
		//attachmentDesc.format = format;
		attachmentDesc.samples = vk::SampleCountFlagBits::e1;
		attachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
		attachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
		attachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		attachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
		attachmentDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentReference colorReference = {};
        colorReference.attachment = 0;
        colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;

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

        vk::RenderPassCreateInfo createInfo = {};
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &attachmentDesc;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpassDescription;
		createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		createInfo.pDependencies = dependencies.data();

        //TODO: check result
        renderPass = UDevice->Make<vk::RenderPass, vk::RenderPassCreateInfo>(createInfo);
    }

    void IrradianceGenerator::CreateFramebuffer()
    {
        GeneratorBase::CreateFramebuffer();

        /*vk::FramebufferCreateInfo createInfo = {};
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &m_pFramebufferImage->GetDescriptor().imageView;
		createInfo.width = texture->baseWidth;
		createInfo.height = texture->baseHeight;
		createInfo.layers = 1;*/
    }

    void IrradianceGenerator::CreateDescriptorSets(uint32_t images)
    {
        GeneratorBase::CreateDescriptorSets(images);
    }
}