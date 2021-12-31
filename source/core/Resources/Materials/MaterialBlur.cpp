#include "MaterialBlur.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Core/Rendering/RendererBase.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core::Descriptor;

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr, vk::RenderPass& rPass)
{
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.shaders = 
    {
        {"../../assets/shaders/main/screenspace.vert"},
        {"../../assets/shaders/postprocess/gaussianblur.frag"}
    };
    MaterialBase::Create(pResMgr, rPass);
}

void MaterialBlur::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    initial.culling = vk::CullModeFlagBits::eFront;
    initial.shaders = 
    {
        {"../../assets/shaders/main/screenspace.vert"},
        {"../../assets/shaders/postprocess/gaussianblur.frag"}
    };
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    MaterialBase::Create(pResMgr);
}

void MaterialBlur::ReCreate()
{
    renderPass = URenderer->GetRenderer(FRendererCreateInfo::ERendererType::ePostProcess)->GetRenderPass();
    MaterialBase::ReCreate();
}

void MaterialBlur::Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
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

void MaterialBlur::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    MaterialBase::Bind(commandBuffer, imageIndex);
}

void MaterialBlur::Cleanup()
{
    MaterialBase::Cleanup();
}

void MaterialBlur::CreateDescriptors(uint32_t images)
{
    // Matrices uniform
    auto matSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment).
    build();

    auto matSet = std::make_unique<VulkanDescriptorSet>();
    matSet->Create(m_pDescriptorPool, matSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(matSet), std::move(matSetLayout));

    // Texture uniform
    auto texSetLayout = VulkanDescriptorSetLayout::Builder().
    addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment).
    build();

    auto texSet = std::make_unique<VulkanDescriptorSet>();
    texSet->Create(m_pDescriptorPool, texSetLayout, images);

    m_pMatDesc->AttachDescriptorSet(std::move(texSet), std::move(texSetLayout));
}
