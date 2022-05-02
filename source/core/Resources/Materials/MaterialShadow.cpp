#include "MaterialShadow.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/ResourceManager.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;
using namespace Engine::Core::Pipeline;

void CMaterialShadow::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    CMaterialBase::create(renderPass, subpass);
}

void CMaterialShadow::reCreate()
{
    m_pPipeline->recreatePipeline();
    CMaterialBase::reCreate();
}

void CMaterialShadow::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    CMaterialBase::update(uboDesc, imageIndex);

    /*auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &uboDesc).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);*/
}

void CMaterialShadow::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    CMaterialBase::bind(commandBuffer, imageIndex);
}

void CMaterialShadow::cleanup()
{
    CMaterialBase::cleanup();
}