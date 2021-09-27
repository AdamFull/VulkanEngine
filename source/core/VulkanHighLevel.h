#pragma once
#include "Pipeline/PipelineConfig.h"
#include "VulkanDevice.h"
#include "Pipeline/GraphicsPipeline.h"
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanTexture.h"
#include "DataTypes/VulkanMesh.h"
#include "VulkanUniform.h"
#include "VulkanRenderer.h"

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

        void AddVulkanTexture(std::string, uint32_t);

        void AddVulkanMesh(std::string);

        void BeginFrame(float);
        //TODO: Dont forget about clean textures
        void Cleanup();

        void ValidateRunAbility();

        //Getters

        inline std::unique_ptr<Device>& GetDevice() { return m_pDevice; }
        inline std::unique_ptr<SwapChain>& GetSwapChain() { return m_pSwapChain; }
        inline std::unique_ptr<GraphicsPipeline>& GetPipeline() { return m_pPipeline; }
        inline std::unique_ptr<UniformBuffer>&  GetUniformBuffer() { return m_pUniform; }
        inline std::unique_ptr<Renderer>&  GetRenderer() { return m_pRenderer; }
    private:
        //Device
        std::unique_ptr<Device> m_pDevice;
        //SwapChain
        std::unique_ptr<SwapChain> m_pSwapChain;

        //Pipeline. Pipelines should be in material. May be shared
        std::unique_ptr<GraphicsPipeline> m_pPipeline;

        //Descriptors
        std::unique_ptr<UniformBuffer> m_pUniform;

        std::unique_ptr<Renderer> m_pRenderer;

        //Loaded texture
        std::unique_ptr<VulkanTextureBase> m_pTexture;
        std::vector<VulkanStaticMesh> m_vMeshes;

        float m_fDeltaTime{0.0f};
    };
}
