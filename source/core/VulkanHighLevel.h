#pragma once
#include "DataTypes/VulkanVertex.h"
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanTexture.h"
#include "DataTypes/VulkanMesh.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

namespace Engine
{
    class WindowHandle;
    class PipelineBase;

    class VulkanHighLevel
    {
    public:
        VulkanHighLevel() = default;
        ~VulkanHighLevel();

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        void AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo);

        //Descriptors
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void CreateCommandBuffers();

        //Uniform
        void CreateUniformBuffers();
        void UpdateUniformBuffer(uint32_t index);

        void AddVulkanTexture(std::string, uint32_t);

        void AddVulkanMesh(std::string, FTransform);

        void DrawFrame(float);

        void CleanupSwapChain();
        //void CleanupTexture();
        void Cleanup();
        void RecreateSwapChain();

        void ValidateRunAbility();

        float GetAspect() { return m_pSwapChain->GetAspectRatio(); }

        void SetProjectionView(glm::mat4 projectionView) { m_matProjectionView = projectionView; }

    private:
        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::unique_ptr<SwapChain> m_pSwapChain;

        //Pipeline
        std::vector<std::unique_ptr<PipelineBase>> m_vPipelines;

        //Descriptors
        vk::DescriptorSetLayout m_DescriptorSetLayout;
        vk::DescriptorPool m_DescriptorPool;
        std::vector<vk::DescriptorSet> m_vDescriptorSets;

        //Uniform
        std::vector<vk::Buffer> m_vUniformBuffers;
        std::vector<vk::DeviceMemory> m_vUniformBuffersMemory;
        glm::mat4 m_matProjectionView;

        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> m_vCommandBuffer;

        glm::vec3 m_vecPosition{0.f, 0.f, 0.f};

        //Loaded texture
        std::unique_ptr<VulkanTextureBase> m_pTexture;
        std::vector<VulkanStaticMesh> m_vMeshes;

        float m_fDeltaTime{0.0f};
    };
}
