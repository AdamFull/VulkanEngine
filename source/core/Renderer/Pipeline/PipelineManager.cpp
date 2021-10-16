#include "PipelineManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanHighLevel.h"
#include "VulkanPipeline.h"

namespace Engine
{
    std::unique_ptr<PipelineManager> PipelineManager::m_pInstance;

    std::unique_ptr<PipelineManager>& PipelineManager::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new PipelineManager());

            uint32_t images = USwapChain->GetImages().size();
            m_pInstance->CreateDescriptorSetLayout();
            m_pInstance->CreateDescriptorPool(images);
            m_pInstance->CreatePipelineLayout(images);
            m_pInstance->CreatePipelineCache();
        }
        return m_pInstance;
    }

    void PipelineManager::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        //m_mPipelines.emplace(EPipelineType::eGraphics, PipelineFactory::CreatePipeline(device, swapchain, EPipelineType::eGraphics));
    }

    void PipelineManager::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            //value->RecreatePipeline(device, swapchain);
        }
    }

    void PipelineManager::Cleanup(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Cleanup(device);
        }
    }

    void PipelineManager::Destroy(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Destroy(device);
        }
    }

    std::unique_ptr<PipelineBase>& PipelineManager::Get(EPipelineType eType)
    {
        auto it = m_mPipelines.find(eType);
        if(it != m_mPipelines.end())
            return it->second;
        return std::unique_ptr<PipelineBase>(nullptr);
    }

    void PipelineManager::CreateDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding diffuseLayoutBinding{};
        diffuseLayoutBinding.binding = 1;
        diffuseLayoutBinding.descriptorCount = 1;
        diffuseLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        diffuseLayoutBinding.pImmutableSamplers = nullptr;
        diffuseLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding normalLayoutBinding{};
        normalLayoutBinding.binding = 2;
        normalLayoutBinding.descriptorCount = 1;
        normalLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        normalLayoutBinding.pImmutableSamplers = nullptr;
        normalLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutBinding specularLayoutBinding{};
        specularLayoutBinding.binding = 3;
        specularLayoutBinding.descriptorCount = 1;
        specularLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        specularLayoutBinding.pImmutableSamplers = nullptr;
        specularLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 4> bindings = {uboLayoutBinding, diffuseLayoutBinding, normalLayoutBinding, specularLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = UDevice->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &descriptorSetLayout);
    }

    void PipelineManager::CreateDescriptorPool(uint32_t images)
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = 1000;
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = 1000;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000;

        descriptorPool = UDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void PipelineManager::CreatePipelineLayout(uint32_t images)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        pipelineLayout = UDevice->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(pipelineLayout && "Pipeline layout was not created");
    }

    void PipelineManager::CreatePipelineCache()
    {
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCache = UDevice->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
    }
}