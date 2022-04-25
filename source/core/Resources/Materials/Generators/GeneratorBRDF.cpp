#include "GeneratorBRDF.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/VulkanInitializers.h"

using namespace Engine::Resources::Material::Generator;
using namespace Engine::Core;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void GeneratorBRDF::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    iFormat = 0x822F; 
    usageFlags = usageFlags | vk::ImageUsageFlagBits::eSampled;

    CreateTextures();
    CreateRenderPass(imageFormat);
    CreateFramebuffer();

    m_pPipeline = PipelineBase::Builder().
    SetRenderPass(renderPass).
    AddShaderStage("../../assets/shaders/main/screenspace.vert").
    AddShaderStage("../../assets/shaders/generators/brdflut_gen.frag").
    Build();
    GeneratorBase::Create(pResMgr);
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

    auto cmdBuf = CommandBuffer();
    auto tempBuffer = cmdBuf.GetCommandBuffer();
    tempBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport = Core::Initializers::Viewport(m_iDimension, m_iDimension);
	vk::Rect2D scissor = Core::Initializers::Scissor(m_iDimension, m_iDimension);

    tempBuffer.setViewport(0, 1, &viewport);
    tempBuffer.setScissor(0, 1, &scissor);
    tempBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pPipeline->GetPipeline());
    tempBuffer.draw(3, 1, 0, 0);
    tempBuffer.endRenderPass();
    cmdBuf.submitIdle();
    m_pGeneratedImage->SetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    m_pGeneratedImage->UpdateDescriptor();
}

void GeneratorBRDF::CreateTextures()
{
    GeneratorBase::CreateTextures();
}