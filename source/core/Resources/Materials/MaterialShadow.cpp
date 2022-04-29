#include "MaterialShadow.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void MaterialShadow::Create(vk::RenderPass& renderPass, uint32_t subpass)
{
    MaterialBase::Create(renderPass, subpass);
}

void MaterialShadow::ReCreate()
{
    m_pPipeline->RecreatePipeline();
    MaterialBase::ReCreate();
}

void MaterialShadow::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    MaterialBase::Update(uboDesc, imageIndex);

    /*auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &uboDesc).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);*/
}

void MaterialShadow::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialShadow::Cleanup()
{
    MaterialBase::Cleanup();
}