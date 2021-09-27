#include "VulkanHighLevel.h"
#include "VulkanSwapChain.h"
#include "VulkanStaticHelper.h"
#include "WindowHandle.h"

namespace Engine
{
    VulkanHighLevel::~VulkanHighLevel()
    {
        //Cleanup();
    }

    void VulkanHighLevel::Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion)
    {
        m_pDevice = std::make_unique<Device>();
        m_pSwapChain = std::make_unique<SwapChain>();
        m_pUniform = std::make_unique<UniformBuffer>();
        m_pPipeline = std::make_unique<GraphicsPipeline>();
        m_pRenderer = std::make_unique<Renderer>();

        m_pDevice->Create(pWindow, pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
        m_pSwapChain->Create(m_pDevice);
        m_pUniform->Create(m_pDevice, m_pSwapChain->GetImages().size());
    }

    void VulkanHighLevel::AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo)
    {
        m_pPipeline->LoadShader(m_pDevice, mShaders);
        createInfo.multisampling.rasterizationSamples = m_pDevice->GetSamples();
        m_pPipeline->Create(createInfo, m_pDevice, m_pSwapChain);
        m_pRenderer->Create(m_pDevice, m_pSwapChain, m_pPipeline, m_pUniform);
    }

    void VulkanHighLevel::AddVulkanMesh(std::string srPath)
    {
        VulkanStaticMesh mesh;
        mesh.LoadStaticMesh(m_pDevice, srPath);
        m_vMeshes.push_back(mesh);
    }

    void VulkanHighLevel::ValidateRunAbility()
    {
        assert(m_pDevice && "Abstract device is not valid.");
        if(VulkanStaticHelper::m_bEnableValidationLayers)
        assert(m_pSwapChain && "Abstract swap chain is not valid.");
        assert(m_pPipeline && "No binded pipelines found.");
    }

    void VulkanHighLevel::AddVulkanTexture(std::string srPath, uint32_t idx)
    {
        //TODO: Add indexed layer of textures
        m_pTexture = std::make_unique<VulkanTextureBase>();
        m_pTexture->Load(m_pDevice, srPath, idx);
    }

    void VulkanHighLevel::BeginFrame(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
        //TODO: Add result handles

        auto commandBuffer = m_pRenderer->BeginFrame(m_pDevice, m_pSwapChain, m_pPipeline, m_pUniform);
        m_pRenderer->BeginRender(commandBuffer, m_pSwapChain);

        m_pPipeline->Bind(commandBuffer);

        for(auto& mesh : m_vMeshes)
        {
            mesh.Bind(commandBuffer);
            mesh.Draw(commandBuffer);
        }

        m_pUniform->Bind(commandBuffer, m_pPipeline->GetPipelineLayout(), m_pRenderer->GetCurrentDescriptorSets());

        m_pRenderer->EndRender(commandBuffer);
        m_pRenderer->EndFrame(m_pDevice, m_pSwapChain, m_pPipeline, m_pUniform);
    }

    void VulkanHighLevel::Cleanup()
    {
        m_pRenderer->CleanupSwapChain(m_pDevice, m_pSwapChain);

        m_pPipeline->Cleanup(m_pDevice);

        //Move it to model
        for(auto& mesh : m_vMeshes)
        {
            mesh.vertices.clear();
            m_pDevice->Destroy(mesh.vertexBuffer);
            m_pDevice->Destroy(mesh.vertexBufferMemory);

            mesh.indices.clear();
            m_pDevice->Destroy(mesh.indexBuffer);
            m_pDevice->Destroy(mesh.indiciesBufferMemory);
        }
        m_vMeshes.clear();

        m_pSwapChain->Destroy(m_pDevice);

        m_pDevice->Cleanup();

        //Engine::PEngine()->PWindow()->Close();
    }
}