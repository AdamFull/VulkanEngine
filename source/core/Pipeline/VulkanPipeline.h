#pragma once
#include "Resources/Meshes/VulkanMesh.h"
#include "PipelineConfig.h"

namespace Engine
{
    class Device;
    class SwapChain;

    struct FShaderCache
    {
        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

    struct FPipeline
    {
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        std::vector<vk::DescriptorSet> vDescriptorSets;

        vk::PipelineLayout layout;
        vk::Pipeline pipeline;
    };

    class PipelineBase
    {
    public:
        virtual ~PipelineBase() {}

        virtual void Create(FPipelineCreateInfo createInfo, std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain);

        void Bind(vk::CommandBuffer& commandBuffer);

        void LoadShader(std::unique_ptr<Device>& device, const std::map<vk::ShaderStageFlagBits, std::string>& mShaders);
        virtual void RecreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) = 0;

        virtual void Cleanup(std::unique_ptr<Device>& device) = 0;

        inline virtual vk::DescriptorSetLayout& GetDescriptorSetLayout() { return data.descriptorSetLayout; }
        inline virtual vk::DescriptorPool& GetDescriptorPool() { return data.descriptorPool; }
        inline virtual vk::PipelineLayout& GetPipelineLayout() { return data.layout; }
        inline virtual vk::Pipeline& GetPipeline() { return data.pipeline; }
        inline virtual vk::PipelineBindPoint& GetBindPoint() { return savedInfo.bindPoint; }
        inline virtual std::vector<vk::DescriptorSet> GetDescriptorSets() { return data.vDescriptorSets; }
        inline virtual vk::DescriptorSet& GetDescriptorSet(uint32_t index) { return data.vDescriptorSets[index]; }
        inline virtual FPipeline& get() { return data; }

    protected:
        virtual void CreateDescriptorSetLayout(std::unique_ptr<Device>& device) {}
        virtual void CreateDescriptorPool(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) {}
        virtual void CreateDescriptorSets(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) {}
        virtual void CreatePipelineLayout(std::unique_ptr<Device>& device) {}
        virtual void CreatePipeline(std::unique_ptr<Device>& device, std::unique_ptr<SwapChain>& swapchain) {}

        void LoadShader(std::unique_ptr<Device>& device, const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(std::unique_ptr<Device>& device, const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
        virtual void RecreateShaders(std::unique_ptr<Device>& device);

        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
        FPipelineCreateInfo savedInfo;

        FPipeline data;
    };
};