#pragma once
#include "Pipeline/PipelineConfig.h"
#include "VulkanDevice.h"
#include "Pipeline/GraphicsPipeline.h"
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanTexture.h"
#include "DataTypes/VulkanMesh.h"
#include "VulkanUniform.h"

namespace Engine
{
    class WindowHandle;
    class SwapChain;

    class VulkanHighLevel
    {
    public:
        VulkanHighLevel() = default;
        ~VulkanHighLevel();

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        void AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo);

        void CreateDescriptorSets();
        void CreateCommandBuffers();

        void AddVulkanTexture(std::string, uint32_t);

        void AddVulkanMesh(std::string);

        void BeginFrame(float);
        void EndFrame();

        void CleanupSwapChain();
        //void CleanupTexture();
        void Cleanup();
        void RecreateSwapChain();

        void ValidateRunAbility();

        void SetProjectionView(glm::mat4 projectionView) { m_matProjectionView = projectionView; }

        //Getters

        inline std::unique_ptr<Device>& GetDevice() { return m_pDevice; }
        inline std::unique_ptr<SwapChain>& GetSwapChain() { return m_pSwapChain; }
        inline std::unique_ptr<PipelineBase>& GetPipeline() { return m_pPipeline; }
        inline std::unique_ptr<UniformBuffer>&  GetUniformBuffer() { return m_pUniform; }
        inline uint32_t GetCurrentImage() { return m_iImageIndex; }
    private:
        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::unique_ptr<SwapChain> m_pSwapChain;

        //Pipeline. Pipelines should be in material. May be shared
        std::unique_ptr<PipelineBase> m_pPipeline;

        //Descriptors
        std::unique_ptr<UniformBuffer> m_pUniform;

        std::vector<vk::DescriptorSet> m_vDescriptorSets;

        //Uniform
        glm::mat4 m_matProjectionView;

        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> m_vCommandBuffer;

        glm::vec3 m_vecPosition{0.f, 0.f, 0.f};

        //Loaded texture
        std::unique_ptr<VulkanTextureBase> m_pTexture;
        std::vector<VulkanStaticMesh> m_vMeshes;

        uint32_t m_iImageIndex{0};

        float m_fDeltaTime{0.0f};
    };
}
