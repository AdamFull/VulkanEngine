#include "PipelineManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "VulkanPipeline.h"

namespace Engine
{
    constexpr std::array<EShaderSet, 4> aShaderSets { EShaderSet::eUI, EShaderSet::eDiffuse, EShaderSet::eSkybox, EShaderSet::ePBR };
    std::unique_ptr<PipelineManager> PipelineManager::m_pInstance;

    std::unique_ptr<PipelineManager>& PipelineManager::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new PipelineManager());
        }
        return m_pInstance;
    }

    void PipelineManager::Create(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        uint32_t images = swapchain->GetImages().size();
        m_pInstance->CreateDescriptorSetLayout(device);
        m_pInstance->CreateDescriptorPool(device, images);
        m_pInstance->CreatePipelineLayout(device, images);
        m_pInstance->CreatePipelineCache(device);

        for(auto& eSet : aShaderSets)
        {
            m_mPipelines.emplace(eSet, PipelineFactory::CreatePipeline(CreateInfo(device, eSet), device, swapchain));
        }
    }

    void PipelineManager::RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain)
    {
        uint32_t images = swapchain->GetImages().size();
        m_pInstance->CreateDescriptorSetLayout(device);
        m_pInstance->CreateDescriptorPool(device, images);
        m_pInstance->CreatePipelineLayout(device, images);
        m_pInstance->CreatePipelineCache(device);

        for(auto& [key, value] : m_mPipelines)
        {
            value->RecreatePipeline(CreateInfo(device, key), device, swapchain);
        }
    }

    void PipelineManager::Cleanup(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Cleanup(device);
        }

        device->Destroy(descriptorSetLayout);
        device->Destroy(descriptorPool);
        device->Destroy(pipelineCache);
        device->Destroy(pipelineLayout);
    }

    void PipelineManager::Destroy(std::unique_ptr<Device> &device)
    {
        for(auto& [key, value] : m_mPipelines)
        {
            value->Destroy(device);
        }

        device->Destroy(descriptorSetLayout);
        device->Destroy(descriptorPool);
        device->Destroy(pipelineCache);
        device->Destroy(pipelineLayout);
    }

    std::shared_ptr<PipelineBase>& PipelineManager::Get(EShaderSet eType)
    {
        auto it = m_mPipelines.find(eType);
        if(it != m_mPipelines.end())
            return it->second;
        return std::shared_ptr<PipelineBase>(nullptr);
    }

    FPipelineCreateInfo PipelineManager::CreateInfo(std::unique_ptr<Device>& device, EShaderSet eSet)
    {
        switch (eSet)
        {
            case EShaderSet::eUI: return PipelineConfig::CreateUIPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eDiffuse: return PipelineConfig::CreateDiffusePipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::eSkybox: return PipelineConfig::CreateSkyboxPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
            case EShaderSet::ePBR: return PipelineConfig::CreatePBRPipeline(device->GetSamples(), pipelineLayout, pipelineCache); break;
        }

        return FPipelineCreateInfo{};
    }

    void PipelineManager::CreateDescriptorSetLayout(std::unique_ptr<Device>& device)
    {
        std::vector<vk::DescriptorSetLayoutBinding> vBindings;
        for(size_t i = 0; i < 5; i++)
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.binding = i;
            binding.descriptorType = (i == 0 ? vk::DescriptorType::eUniformBuffer : vk::DescriptorType::eCombinedImageSampler);
            binding.descriptorCount = 1;
            binding.pImmutableSamplers = nullptr;
            binding.stageFlags = (i == 0 ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment);
            vBindings.emplace_back(binding);
        }
        
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(vBindings.size());;
        createInfo.pBindings = vBindings.data();

        //TODO: check result
        auto result = device->GetLogical()->createDescriptorSetLayout(&createInfo, nullptr, &descriptorSetLayout);
    }

    void PipelineManager::CreateDescriptorPool(std::unique_ptr<Device>& device, uint32_t images)
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

        descriptorPool = device->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }

    void PipelineManager::CreatePipelineLayout(std::unique_ptr<Device>& device, uint32_t images)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        pipelineLayout = device->Make<vk::PipelineLayout, vk::PipelineLayoutCreateInfo>(pipelineLayoutInfo);
        assert(pipelineLayout && "Pipeline layout was not created");
    }

    void PipelineManager::CreatePipelineCache(std::unique_ptr<Device>& device)
    {
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCache = device->GetLogical()->createPipelineCache(pipelineCacheCreateInfo);
    }
}