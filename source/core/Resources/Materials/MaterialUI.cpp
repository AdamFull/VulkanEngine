#include "MaterialUI.h"
#include "Core/Overlay/ImguiOverlay.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialUI::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.vertexInputDesc = Core::VertexUI::getBindingDescription();
    initial.vertexAtribDesc = Core::VertexUI::getAttributeDescriptions();
    initial.shaders = 
    {
        {vk::ShaderStageFlagBits::eVertex, "../../assets/shaders/main/ui/vert.spv"},
        {vk::ShaderStageFlagBits::eFragment, "../../assets/shaders/main/ui/frag.spv"}
    };
    renderPass = USwapChain->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialUI::ReCreate()
{
    renderPass = USwapChain->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialUI::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    MaterialBase::Update(uboDesc, imageIndex);

    auto bufferInfo = VulkanDescriptorWriter().
    WriteBuffer(0, m_pMatDesc->GetSetLayout(0)->GetBindings(), &uboDesc).
    Build();
    m_pMatDesc->Update(0, imageIndex, bufferInfo);

    auto imageInfo = VulkanDescriptorWriter().
    WriteImage(0, m_pMatDesc->GetSetLayout(1)->GetBindings(), &m_mTextures[ETextureAttachmentType::eDiffuseAlbedo]).
    Build();
    m_pMatDesc->Update(1, imageIndex, imageInfo);
}

void MaterialUI::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialUI::Cleanup()
{
    MaterialBase::Cleanup();
}

void MaterialUI::CreateDescriptors(uint32_t images)
{
    // Matrices uniform
    auto matSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex).
    build();

    auto matSet = std::make_unique<VulkanDescriptorSet>();
    matSet->Create(m_pDescriptorPool, matSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));

    // Texture uniform
    auto texSetLayout = VulkanDescriptorSetLayout::Builder().addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).build();

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}