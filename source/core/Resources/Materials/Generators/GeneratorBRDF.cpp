#include "GeneratorBRDF.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
;

void GeneratorBRDF::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.shaders = 
    {
        {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/screenspace/vert.spv"},
        {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/generators/brdflut_gen/frag.spv"}
    };
    finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    iFormat = 0x822F; 
    usageFlags = usageFlags | vk::ImageUsageFlagBits::eSampled;
    GeneratorBase::Create(pResMgr);
}

void GeneratorBRDF::CreateDescriptors(uint32_t images)
{
    GeneratorBase::CreateDescriptors(images);

    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build();

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);
    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}

void GeneratorBRDF::Generate(std::shared_ptr<Mesh::MeshBase> pMesh)
{
    GeneratorBase::Generate(pMesh);

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
    vk::Viewport viewport = Core::Initializers::Viewport(m_iDimension, m_iDimension);
	vk::Rect2D scissor = Core::Initializers::Scissor(m_iDimension, m_iDimension);

    tempBuffer.setViewport(0, 1, &viewport);
    tempBuffer.setScissor(0, 1, &scissor);
    tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pPipeline->GetPipeline());
    tempBuffer.draw(3, 1, 0, 0);
    tempBuffer.endRenderPass();
    UDevice->EndSingleTimeCommands(tempBuffer);
    m_pGeneratedImage->SetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    m_pGeneratedImage->UpdateDescriptor();
}

void GeneratorBRDF::CreateTextures()
{
    GeneratorBase::CreateTextures();
}