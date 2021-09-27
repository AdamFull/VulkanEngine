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
        m_pDevice->Create(pWindow, pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
        m_pSwapChain = std::make_unique<SwapChain>();
        m_pSwapChain->Create(m_pDevice);
        m_pUniform = std::make_unique<UniformBuffer>();
        m_pUniform->Create(m_pDevice, m_pSwapChain->GetImages().size());
    }

    void VulkanHighLevel::AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo)
    {
        m_pPipeline = std::make_unique<GraphicsPipeline>();
        m_pPipeline->LoadShader(m_pDevice, mShaders);
        createInfo.multisampling.rasterizationSamples = m_pDevice->GetSamples();
        m_pPipeline->Create(createInfo, m_pDevice, m_pSwapChain);

        CreateDescriptorSets();
    }

    void VulkanHighLevel::CreateDescriptorSets()
    {
        uint32_t images = m_pSwapChain->GetImages().size();
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(images, m_pPipeline->GetDescriptorSetLayout());

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = m_pPipeline->GetDescriptorPool();
        allocInfo.descriptorSetCount = images;
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        m_vDescriptorSets.resize(images);

        auto result = m_pDevice->GetLogical()->allocateDescriptorSets(&allocInfo, m_vDescriptorSets.data());

        for (size_t i = 0; i < images; i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniform->GetUniformBuffers().at(i);
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(FUniformData);

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].dstSet = m_vDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].dstSet = m_vDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &m_pTexture->descriptor;

            m_pDevice->GetLogical()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanHighLevel::CreateCommandBuffers()
    {
        assert(m_pDevice && "Cannot create command buffers, cause logical device is not valid.");
        assert(m_pSwapChain && "Cannot create command buffers, cause swap chain is not valid.");
        assert(m_pPipeline && "Cannot create command buffers, cause pipeline is not valid.");
        m_vCommandBuffer.resize(m_pSwapChain->GetFramebuffers().size());

        m_vCommandBuffer = m_pDevice->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)m_vCommandBuffer.size());
        assert(!m_vCommandBuffer.empty() && "Created command buffers is not valid.");

        for (size_t i = 0; i < m_vCommandBuffer.size(); i++)
        {
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

            m_vCommandBuffer[i].begin(beginInfo);

            vk::RenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.renderPass = m_pSwapChain->GetRenderPass();
            renderPassInfo.framebuffer = m_pSwapChain->GetFramebuffers().at(i);
            renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
            renderPassInfo.renderArea.extent =  m_pSwapChain->GetExtent();

            std::array<vk::ClearValue, 2> clearValues{};
            clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            m_vCommandBuffer[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

            m_pPipeline->Bind(m_vCommandBuffer[i]);

            for(auto& mesh : m_vMeshes)
            {
                mesh.Bind(m_vCommandBuffer[i]);
                mesh.Draw(m_vCommandBuffer[i]);
            }

            m_pUniform->Bind(m_vCommandBuffer[i], m_pPipeline->GetPipelineLayout(), m_vDescriptorSets[i]);

            m_vCommandBuffer[i].endRenderPass();

            m_vCommandBuffer[i].end();
        }
    }

    void VulkanHighLevel::AddVulkanMesh(std::string srPath)
    {
        VulkanStaticMesh mesh;
        mesh.LoadStaticMesh(m_pDevice, srPath);
        m_vMeshes.push_back(mesh);
    }

    void VulkanHighLevel::CleanupSwapChain()
    {
        m_pSwapChain->Cleanup(m_pDevice);
        m_pDevice->Destroy(m_vCommandBuffer);
    }

    void VulkanHighLevel::RecreateSwapChain()
    {
        m_pDevice->GPUWait();

        CleanupSwapChain();
        m_pSwapChain->ReCreate(m_pDevice);
        
        m_pPipeline->RecreatePipeline(m_pDevice, m_pSwapChain);

        CreateCommandBuffers();
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

        try
        {
            m_pSwapChain->AcquireNextImage(m_pDevice, &m_iImageIndex);
        }
        catch (vk::OutOfDateKHRError err)
        {
            RecreateSwapChain();
            return;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }
    }
        
    void VulkanHighLevel::EndFrame()
    {
        vk::Result resultPresent;
        try
        {
            resultPresent = m_pSwapChain->SubmitCommandBuffers(m_pDevice, &m_vCommandBuffer[m_iImageIndex], &m_iImageIndex);
        }
        catch (vk::OutOfDateKHRError err)
        {
            resultPresent = vk::Result::eErrorOutOfDateKHR;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR || WindowHandle::m_bWasResized)
        {
            WindowHandle::m_bWasResized = false;
            RecreateSwapChain();
            return;
        }
    }

    void VulkanHighLevel::Cleanup()
    {
        CleanupSwapChain();

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