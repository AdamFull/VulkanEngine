#include "VulkanHighLevel.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUniform.h"
#include "VulkanRenderer.h"
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
        m_pRenderer = std::make_unique<Renderer>();

        m_pDevice->Create(pWindow, pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
        m_pSwapChain->Create(m_pDevice);
        m_pUniform->Create(m_pDevice, m_pSwapChain->GetImages().size());
        m_pRenderer->Create(m_pDevice, m_pSwapChain);
    }

    /*void VulkanHighLevel::AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo)
    {
        m_pPipeline->LoadShader(m_pDevice, mShaders);
        createInfo.multisampling.rasterizationSamples = m_pDevice->GetSamples();
        m_pPipeline->Create(createInfo, m_pDevice, m_pSwapChain);
        m_pRenderer->Create(m_pDevice, m_pSwapChain, m_pPipeline, m_pUniform);
    }*/

    /*void VulkanHighLevel::AddVulkanMesh(std::string srPath)
    {
        VulkanStaticMesh mesh;
        mesh.LoadStaticMesh(m_pDevice, srPath);
        m_vMeshes.push_back(mesh);
    }*/

    void VulkanHighLevel::ValidateRunAbility()
    {
        /*assert(m_pDevice && "Abstract device is not valid.");
        if(VulkanStaticHelper::m_bEnableValidationLayers)
        assert(m_pSwapChain && "Abstract swap chain is not valid.");
        assert(m_pPipeline && "No binded pipelines found.");*/
    }

    /*void VulkanHighLevel::AddVulkanTexture(std::string srPath, uint32_t idx)
    {
        //TODO: Add indexed layer of textures
        m_pTexture = std::make_unique<VulkanTextureBase>();
        m_pTexture->Load(m_pDevice, srPath, idx);
    }*/

    void VulkanHighLevel::BeginFrame(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
        //TODO: Add result handles

        m_pRoot->Update(fDeltaTime);

        vk::CommandBuffer commandBuffer;
        try { commandBuffer = m_pRenderer->BeginFrame(m_pDevice); }
        catch (vk::OutOfDateKHRError err)
        {
            RecreateSwapChain();
            return;
        }
        catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

        m_pRenderer->BeginRender(commandBuffer);

        m_pRoot->Render(fDeltaTime, commandBuffer);
        /*auto camera = std::static_pointer_cast<CameraBase>(m_pRoot->Find("default_camera"));
            auto projectionView = camera->GetProjection() * camera->GetView();

            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            m_pRender->GetUniformBuffer()->UpdateUniformBuffer(m_pRender->GetDevice(), m_pRender->GetRenderer()->GetImageIndex(), projectionView * model);*/
        //Rendering
        //m_pPipeline->Bind(commandBuffer);

        /*for(auto& mesh : m_vMeshes)
        {
            mesh.Bind(commandBuffer);
            mesh.Draw(commandBuffer);
        }*/

        //m_pUniform->Bind(commandBuffer, m_pPipeline->GetPipelineLayout(), m_pRenderer->GetCurrentDescriptorSets());

        m_pRenderer->EndRender(commandBuffer);

        vk::Result resultPresent;
        try { resultPresent = m_pRenderer->EndFrame(m_pDevice); }
        catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
        catch (vk::SystemError err) { throw std::runtime_error("failed to present swap chain image!"); }

        if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR || WindowHandle::m_bWasResized)
        {
            WindowHandle::m_bWasResized = false;
            RecreateSwapChain();
            return;
        }
       
    }

    void VulkanHighLevel::RecreateSwapChain()
    {
        m_pDevice->GPUWait();

        CleanupSwapChain();
        m_pSwapChain->ReCreate(m_pDevice);
        
        //TODO: add method ReCreate to render object
        m_pRoot->ReCreate(m_pDevice);

        m_pRenderer->ReCreate(m_pDevice);
    }

    void VulkanHighLevel::CleanupSwapChain()
    {
        m_pSwapChain->Cleanup(m_pDevice);
    }

    void VulkanHighLevel::Cleanup()
    {
        CleanupSwapChain();

        //Cleanup from scene graph
        //m_pPipeline->Cleanup(m_pDevice);
        m_pRoot->ReCreate(m_pDevice);

        //Move it to model
        /*for(auto& mesh : m_vMeshes)
        {
            mesh.vertices.clear();
            m_pDevice->Destroy(mesh.vertexBuffer);
            m_pDevice->Destroy(mesh.vertexBufferMemory);

            mesh.indices.clear();
            m_pDevice->Destroy(mesh.indexBuffer);
            m_pDevice->Destroy(mesh.indiciesBufferMemory);
        }
        m_vMeshes.clear();*/

        m_pSwapChain->Destroy(m_pDevice);

        m_pDevice->Cleanup();

        //Engine::PEngine()->PWindow()->Close();
    }
}