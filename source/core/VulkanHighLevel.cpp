#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include "WindowHandle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

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
        CreateUniformBuffers();
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
    }
    
    void VulkanHighLevel::CreateDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo createInfo{};
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        createInfo.pBindings = bindings.data();

        //TODO: check result
        auto result = m_pDevice->get().logical->createDescriptorSetLayout(&createInfo, nullptr, &m_DescriptorSetLayout);
    }

    void VulkanHighLevel::CreateDescriptorPool()
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(m_pSwapChain->GetImagesSize());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(m_pSwapChain->GetImagesSize());

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(m_pSwapChain->GetImagesSize());

        //TODO:: add result checking
        m_DescriptorPool = m_pDevice->Make<vk::DescriptorPool, vk::DescriptorPoolCreateInfo>(poolInfo);
    }
    
    void VulkanHighLevel::CreateDescriptorSets()
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(m_pSwapChain->GetImagesSize(), m_DescriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_pSwapChain->GetImagesSize());
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        m_vDescriptorSets.resize(m_pSwapChain->GetImagesSize());

        auto result = m_pDevice->get().logical->allocateDescriptorSets(&allocInfo, m_vDescriptorSets.data());

        for (size_t i = 0; i < m_pSwapChain->GetImagesSize(); i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_vUniformBuffers[i];
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

            m_pDevice->get().logical->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanHighLevel::CreateUniformBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(FUniformData);

        m_vUniformBuffers.resize(m_pSwapChain->GetImagesSize());
        m_vUniformBuffersMemory.resize(m_pSwapChain->GetImagesSize());

        for (size_t i = 0; i < m_pSwapChain->GetImagesSize(); i++) 
        {
            m_pDevice->CreateOnDeviceBuffer
            (
                bufferSize, 
                vk::BufferUsageFlagBits::eUniformBuffer, 
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
                m_vUniformBuffers[i], 
                m_vUniformBuffersMemory[i]
            );
        }
    }

    void VulkanHighLevel::UpdateUniformBuffer(uint32_t index)
    {
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        /*glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);
        proj[1][1] *= -1;
        auto projectionView = proj * view;*/

        FUniformData ubo{};
        ubo.transform = m_matProjectionView * model;

        m_pDevice->MoveToMemory(&ubo, m_vUniformBuffersMemory[index], sizeof(ubo));
    }

    void VulkanHighLevel::AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo)
    {
        CreateDescriptorSets();
        auto newPipeline = std::make_unique<PipelineBase>();
        newPipeline->LoadShader(m_pDevice, mShaders);
        createInfo.multisampling.rasterizationSamples = m_pDevice->get().msaaSamples;
        newPipeline->Create(createInfo, m_pDevice, m_pSwapChain ,m_DescriptorSetLayout);
        m_vPipelines.emplace_back(std::move(newPipeline));
    }

    void VulkanHighLevel::CreateCommandBuffers()
    {
        assert(m_pDevice && "Cannot create command buffers, cause logical device is not valid.");
        assert(m_pSwapChain && "Cannot create command buffers, cause swap chain is not valid.");
        assert(!m_vPipelines.empty() && "Cannot create command buffers, cause pipeline is not valid.");
        m_vCommandBuffer.resize(m_pSwapChain->GetFBSize());

        m_vCommandBuffer = m_pDevice->CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, (uint32_t)m_vCommandBuffer.size());
        assert(!m_vCommandBuffer.empty() && "Created command buffers is not valid.");

        for (size_t i = 0; i < m_vCommandBuffer.size(); i++)
        {
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

            m_vCommandBuffer[i].begin(beginInfo);

            vk::RenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.renderPass = m_pSwapChain->get().renderPass;
            renderPassInfo.framebuffer = m_pSwapChain->get().vFramebuffers[i];
            renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
            renderPassInfo.renderArea.extent =  m_pSwapChain->get().extent;

            std::array<vk::ClearValue, 2> clearValues{};
            clearValues[0].color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            m_vCommandBuffer[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

            m_vPipelines[0]->Bind(m_vCommandBuffer[i]);

            for(auto& mesh : m_vMeshes)
            {
                mesh.Bind(m_vCommandBuffer[i]);
                mesh.Draw(m_vCommandBuffer[i]);
            }

            m_vCommandBuffer[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_vPipelines[0]->get().layout, 0, 1, &m_vDescriptorSets[i], 0, nullptr);

            m_vCommandBuffer[i].endRenderPass();

            m_vCommandBuffer[i].end();
        }
    }

    void VulkanHighLevel::AddVulkanMesh(std::string srPath, FTransform transform)
    {
        VulkanStaticMesh mesh;
        mesh.LoadStaticMesh(m_pDevice, srPath, transform);
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
        
        for(auto& pipeline : m_vPipelines)
        {
            pipeline->RecreatePipeline(m_pDevice, m_pSwapChain, m_DescriptorSetLayout);
        }

        CreateCommandBuffers();
    }

    void VulkanHighLevel::ValidateRunAbility()
    {
        assert(m_pDevice && "Abstract device is not valid.");
        if(VulkanStaticHelper::m_bEnableValidationLayers)
        assert(m_pSwapChain && "Abstract swap chain is not valid.");
        assert(!m_vPipelines.empty() && "No binded pipelines found.");
    }

    void VulkanHighLevel::AddVulkanTexture(std::string srPath, uint32_t idx)
    {
        //TODO: Add indexed layer of textures
        m_pTexture = std::make_unique<VulkanTextureBase>();

        int w, h, c;
        unsigned char* raw_data = stbi_load(srPath.c_str(), &w, &h, &c, STBI_rgb_alpha);

        if (!raw_data)
        {
            //TODO: check result
        }

        m_pTexture->width = static_cast<uint32_t>(w);
        m_pTexture->height = static_cast<uint32_t>(h);
        m_pTexture->channels = static_cast<uint32_t>(c);
        m_pTexture->mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_pTexture->width, m_pTexture->height)))) + 1;

        vk::DeviceSize imgSize = m_pTexture->width * m_pTexture->height * 4;
        auto compiledSize = vk::Extent3D{m_pTexture->width, m_pTexture->height, m_pTexture->channels};

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        m_pDevice->CreateOnDeviceBuffer(imgSize, vk::BufferUsageFlagBits::eTransferSrc,
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                             stagingBuffer,
                             stagingBufferMemory);

        m_pDevice->MoveToMemory(raw_data, stagingBufferMemory , imgSize);

        stbi_image_free(raw_data);

        m_pDevice->CreateImage(m_pTexture->image, m_pTexture->deviceMemory, compiledSize, m_pTexture->mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, 
                    vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
                    vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        m_pDevice->TransitionImageLayout(m_pTexture->image, m_pTexture->mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        m_pDevice->CopyBufferToImage(stagingBuffer, m_pTexture->image, compiledSize);
        m_pDevice->GenerateMipmaps(m_pTexture->image, m_pTexture->mipLevels, vk::Format::eR8G8B8A8Srgb, compiledSize, vk::ImageAspectFlagBits::eColor);

        m_pDevice->Destroy(stagingBuffer);
        m_pDevice->Destroy(stagingBufferMemory);

        m_pTexture->view = m_pDevice->CreateImageView(m_pTexture->image, m_pTexture->mipLevels, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        m_pDevice->CreateSampler(m_pTexture->sampler, m_pTexture->mipLevels);
        m_pTexture->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        m_pTexture->UpdateDescriptor();
    }

    void VulkanHighLevel::DrawFrame(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
        //TODO: Add result handles

        uint32_t imageIndex;
        try
        {
            m_pSwapChain->AcquireNextImage(m_pDevice, &imageIndex);
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

        UpdateUniformBuffer(imageIndex);

        vk::Result resultPresent;
        try
        {
            resultPresent = m_pSwapChain->SubmitCommandBuffers(m_pDevice, &m_vCommandBuffer[imageIndex], &imageIndex);
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

        for(auto& pipeline : m_vPipelines)
        {
            pipeline->Cleanup(m_pDevice);
        }

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