#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include "WindowHandle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace Engine
{
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, callback, pAllocator);
        }
    }

    VulkanHighLevel::~VulkanHighLevel()
    {
        //Cleanup();
    }

    void VulkanHighLevel::Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion)
    {
        CreateInstance(pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion);
        CreateDebugCallback();
        CreateSurface(pWindow);
        CreateDevice();
        CreateSwapChain();
        CreateSwapChainImageViews();
        CreateRenderPass();
        CreateCommandPool();
        CreateMSAAResources();
        CreateDepthResources();
        CreateFrameBuffers();
        CreateUniformBuffers();
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateSyncObjects();
    }

    void VulkanHighLevel::CreateInstance(const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion)
    {
        if (VulkanStaticHelper::m_bEnableValidationLayers && !VulkanStaticHelper::CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        auto appInfo = vk::ApplicationInfo(
            pApplicationName, applicationVersion,
            pEngineName, engineVersion,
            apiVersion);

        auto extensions = VulkanStaticHelper::GetRequiredExtensions();

        auto createInfo = vk::InstanceCreateInfo(
            vk::InstanceCreateFlags(),
            &appInfo,
            0, nullptr,                                                 // enabled layers
            static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
        );

        if (VulkanStaticHelper::m_bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanStaticHelper::m_vValidationLayers.size());
            createInfo.ppEnabledLayerNames = VulkanStaticHelper::m_vValidationLayers.data();
        }

        m_pVkInstance = vk::createInstanceUnique(createInfo, nullptr);
        assert(m_pVkInstance && "Vulkan instance was not created.");
    }

    void VulkanHighLevel::CreateDebugCallback()
    {
        if (!VulkanStaticHelper::m_bEnableValidationLayers)
            return;

        auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            ValidationCallback,
            nullptr);

        // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
        if (CreateDebugUtilsMessengerEXT(*m_pVkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &m_pVkDebugUtils) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug callback!");
        }
        //m_pVkDebugUtils = m_pVkInstance->createDebugUtilsMessengerEXTUnique(createInfo);
    }

    void VulkanHighLevel::CreateSurface(std::unique_ptr<WindowHandle>& pWindow)
    {
        assert(m_pVkInstance && "Unable to create surface, cause vulkan instance is not valid");
        pWindow->CreateWindowSurface(m_pVkInstance, m_pSurface);
        assert(m_pSurface && "Surface creation failed");
    }

    void VulkanHighLevel::CreateDevice()
    {
        assert(m_pVkInstance && "Unable to create ligical device, cause vulkan instance is not valid");
        m_PhysicalDevice = GetPhysicalDevice();
        assert(m_PhysicalDevice && "Physical device is not avaliable");

        msaaSamples = GetMaxUsableSampleCount();
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back({vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority});
        }

        auto deviceFeatures = vk::PhysicalDeviceFeatures();
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        auto createInfo = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(),
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data());

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanStaticHelper::m_vDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = VulkanStaticHelper::m_vDeviceExtensions.data();

        if (VulkanStaticHelper::m_bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanStaticHelper::m_vValidationLayers.size());
            createInfo.ppEnabledLayerNames = VulkanStaticHelper::m_vValidationLayers.data();
        }

        m_pDevice = m_PhysicalDevice.createDeviceUnique(createInfo);
        assert(m_pDevice && "Cannot create logical device.");

        m_qGraphicsQueue = m_pDevice->getQueue(indices.graphicsFamily.value(), 0);
        assert(m_qGraphicsQueue && "Failed while getting graphics queue");
        m_qPresentQueue = m_pDevice->getQueue(indices.presentFamily.value(), 0);
        assert(m_qPresentQueue && "Failed while getting present queue");
    }

    uint32_t VulkanHighLevel::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = m_PhysicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }

    QueueFamilyIndices VulkanHighLevel::FindQueueFamilies(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices;

        auto queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, m_pSurface))
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    vk::PhysicalDevice VulkanHighLevel::GetPhysicalDevice()
    {
        auto device = m_pVkInstance->enumeratePhysicalDevices().front();
        if(device && IsDeviceSuitable(device))
        {
            return device;
        }
        return nullptr;
    }

    std::vector<vk::PhysicalDevice> VulkanHighLevel::GetAvaliablePhysicalDevices()
    {
        auto devices = m_pVkInstance->enumeratePhysicalDevices();
        std::vector<vk::PhysicalDevice> output_devices;
        if (devices.size() == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        for (const auto &device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                output_devices.emplace_back(device);
            }
        }

        if (output_devices.size() == 0)
        {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
        return output_devices;
    }

    bool VulkanHighLevel::IsDeviceSuitable(const vk::PhysicalDevice &device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = VulkanStaticHelper::CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        vk::PhysicalDeviceFeatures supportedFeatures{};
        supportedFeatures = device.getFeatures();

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.sampleRateShading;
    }

    void VulkanHighLevel::CreateOnDeviceBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        buffer = m_pDevice->createBuffer(bufferInfo);
        assert(buffer && "On device buffer was not created");

        vk::MemoryRequirements memRequirements = m_pDevice->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        bufferMemory = m_pDevice->allocateMemory(allocInfo);
        assert(bufferMemory && "Buffer memory was not allocated");

        m_pDevice->bindBufferMemory(buffer, bufferMemory, 0);
    }

    void VulkanHighLevel::CopyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferCopy copyRegion = {};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    vk::CommandBuffer VulkanHighLevel::BeginSingleTimeCommands()
    {
        vk::CommandBuffer commandBuffer = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, m_pCommandPool, 1)[0];

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VulkanHighLevel::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        m_qGraphicsQueue.submit(submitInfo, nullptr);
        m_qGraphicsQueue.waitIdle();

        m_pDevice->freeCommandBuffers(m_pCommandPool, commandBuffer);
    }

    vk::SampleCountFlagBits VulkanHighLevel::GetMaxUsableSampleCount() 
    {
        vk::PhysicalDeviceProperties physicalDeviceProperties;
        m_PhysicalDevice.getProperties(&physicalDeviceProperties);

        vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
        if (counts & vk::SampleCountFlagBits::e8 ) { return vk::SampleCountFlagBits::e8 ; }
        if (counts & vk::SampleCountFlagBits::e4 ) { return vk::SampleCountFlagBits::e4 ; }
        if (counts & vk::SampleCountFlagBits::e2 ) { return vk::SampleCountFlagBits::e2 ; }

        return vk::SampleCountFlagBits::e1;
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
        auto result = m_pDevice->createDescriptorSetLayout(&createInfo, nullptr, &m_DescriptorSetLayout);
    }

    void VulkanHighLevel::CreateDescriptorPool()
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(m_vSwapChainImages.size());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(m_vSwapChainImages.size());

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(m_vSwapChainImages.size());

        //TODO:: add result checking
        m_DescriptorPool = m_pDevice->createDescriptorPool(poolInfo);
    }
    
    void VulkanHighLevel::CreateDescriptorSets()
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(m_vSwapChainImages.size(), m_DescriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_vSwapChainImages.size());
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();

        m_vDescriptorSets.resize(m_vSwapChainImages.size());

        auto result = m_pDevice->allocateDescriptorSets(&allocInfo, m_vDescriptorSets.data());

        for (size_t i = 0; i < m_vSwapChainImages.size(); i++)
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

            m_pDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanHighLevel::CreateUniformBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(FUniformData);

        m_vUniformBuffers.resize(m_vSwapChainImages.size());
        m_vUniformBuffersMemory.resize(m_vSwapChainImages.size());

        for (size_t i = 0; i < m_vSwapChainImages.size(); i++) 
        {
            CreateOnDeviceBuffer
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

        MoveToMemory(&ubo, m_vUniformBuffersMemory[index], sizeof(ubo));
    }

    void VulkanHighLevel::AddPipeline(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders, FPipelineCreateInfo createInfo)
    {
        CreateDescriptorSets();
        auto newPipeline = std::make_unique<PipelineBase>();
        newPipeline->LoadShader(m_pDevice, mShaders);
        createInfo.multisampling.rasterizationSamples = msaaSamples;
        newPipeline->Create(createInfo, m_pDevice, m_DescriptorSetLayout, m_pRenderPass);
        m_vPipelines.emplace_back(std::move(newPipeline));
    }

    void VulkanHighLevel::CreateFrameBuffers()
    {
        assert(!m_vSwapChainImageViews.empty() && "Cannot create framebuffers, cause swap chain image views are empty.");
        assert(m_pDevice && "Cannot create framebuffers, cause logical device is not valid.");
        assert(m_pRenderPass && "Cannot create framebuffers, cause render pass is not valid.");
        assert(m_MSAAImageView && "Cannot create framebuffers, cause msaa image view is not valid.");
        assert(m_DepthImageView && "Cannot create framebuffers, cause depth image view is not valid.");

        m_vSwapChainFrameBuffers.resize(m_vSwapChainImageViews.size());

        for (size_t i = 0; i < m_vSwapChainImageViews.size(); i++)
        {
            std::array<vk::ImageView, 3> attachments =
            {
                m_MSAAImageView,
                m_DepthImageView,
                m_vSwapChainImageViews[i]
            };

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass = m_pRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_SwapChainExtent.width;
            framebufferInfo.height = m_SwapChainExtent.height;
            framebufferInfo.layers = 1;

            m_vSwapChainFrameBuffers[i] = m_pDevice->createFramebuffer(framebufferInfo);
            assert(m_vSwapChainFrameBuffers[i] && "Failed while creating framebuffer");
        }
    }
    
    void VulkanHighLevel::CreateCommandPool()
    {
        assert(m_PhysicalDevice && "Cannot create command pool, cause physical device is not valid.");
        assert(m_pDevice && "Cannot create command pool, cause logical device is not valid.");
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        m_pCommandPool = m_pDevice->createCommandPool(poolInfo);
        assert(m_pCommandPool && "Failed while creating command pool");
    }

    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> 
    VulkanHighLevel::CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool& pool, uint32_t count)
    {
        assert(m_pDevice && "Cannot create command buffer, cause logical device is not valid.");
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool = pool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = count;

        //TODO: Handle error
        return m_pDevice->allocateCommandBuffers(allocInfo);
    }

    void VulkanHighLevel::CreateCommandBuffers()
    {
        assert(m_pDevice && "Cannot create command buffers, cause logical device is not valid.");
        assert(m_pCommandPool && "Cannot create command buffers, cause command pool is not valid.");
        assert(m_pRenderPass && "Cannot create command buffers, cause render pass is not valid.");
        assert(!m_vPipelines.empty() && "Cannot create command buffers, cause pipeline is not valid.");
        m_vCommandBuffer.resize(m_vSwapChainFrameBuffers.size());

        m_vCommandBuffer = CreateCommandBuffer(vk::CommandBufferLevel::ePrimary, m_pCommandPool, (uint32_t)m_vCommandBuffer.size());
        assert(!m_vCommandBuffer.empty() && "Created command buffers is not valid.");

        for (size_t i = 0; i < m_vCommandBuffer.size(); i++)
        {
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

            m_vCommandBuffer[i].begin(beginInfo);

            vk::RenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.renderPass = m_pRenderPass;
            renderPassInfo.framebuffer = m_vSwapChainFrameBuffers[i];
            renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
            renderPassInfo.renderArea.extent = m_SwapChainExtent;

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

    void VulkanHighLevel::CreateVertexBuffer(std::vector<Vertex>& vertices, vk::Buffer& vertexBuffer, vk::DeviceMemory& vertexBufferMemory)
    {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        MoveToMemory(vertices.data(), stagingBufferMemory, bufferSize);
        CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
        CopyOnDeviceBuffer(stagingBuffer, vertexBuffer, bufferSize);

        m_pDevice->destroyBuffer(stagingBuffer);
        m_pDevice->freeMemory(stagingBufferMemory);
    }

    void VulkanHighLevel::CreateIndexBuffer(std::vector<uint32_t>& indices, vk::Buffer& indexBuffer, vk::DeviceMemory& indiciesBufferMemory)
    {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        MoveToMemory(indices.data(), stagingBufferMemory, bufferSize);
        CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indiciesBufferMemory);
        CopyOnDeviceBuffer(stagingBuffer, indexBuffer, bufferSize);

        m_pDevice->destroyBuffer(stagingBuffer);
        m_pDevice->freeMemory(stagingBufferMemory);
    }

    void VulkanHighLevel::AddVulkanMesh(std::string srPath, FTransform transform)
    {
        VulkanStaticMesh mesh;
        mesh.LoadStaticMesh(srPath, transform);
        if(!mesh.vertices.empty())
            CreateVertexBuffer(mesh.vertices, mesh.vertexBuffer, mesh.vertexBufferMemory);
        if(!mesh.indices.empty())
            CreateIndexBuffer(mesh.indices, mesh.indexBuffer, mesh.indiciesBufferMemory);

        m_vMeshes.push_back(mesh);
    }

    SwapChainSupportDetails VulkanHighLevel::QuerySwapChainSupport(const vk::PhysicalDevice &device)
    {
        SwapChainSupportDetails details;
        details.capabilities = device.getSurfaceCapabilitiesKHR(m_pSurface);
        details.formats = device.getSurfaceFormatsKHR(m_pSurface);
        details.presentModes = device.getSurfacePresentModesKHR(m_pSurface);

        return details;
    }

    vk::Extent2D VulkanHighLevel::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            vk::Extent2D actualExtent = {static_cast<uint32_t>(WindowHandle::m_iWidth), static_cast<uint32_t>(WindowHandle::m_iHeight)};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    void VulkanHighLevel::CreateSwapChain()
    {
        assert(m_pSurface && "Cannot create swap chain, cause surface is not valid.");
        assert(m_PhysicalDevice && "Cannot create swap chain, cause physical device is not valid.");
        assert(m_pDevice && "Cannot create swap chain, cause logical device is not valid.");
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

        vk::SurfaceFormatKHR surfaceFormat = VulkanStaticHelper::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = VulkanStaticHelper::ChooseSwapPresentMode(swapChainSupport.presentModes);
        m_SwapChainExtent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.maxImageCount;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            m_pSurface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            m_SwapChainExtent,
            1, // imageArrayLayers
            vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        m_pSwapchain = m_pDevice->createSwapchainKHR(createInfo);
        assert(m_pSwapchain && "Swap chain was not created");

        m_vSwapChainImages = m_pDevice->getSwapchainImagesKHR(m_pSwapchain);
        assert(!m_vSwapChainImages.empty() && "Swap chain images was not created");

        m_SwapChainImageFormat = surfaceFormat.format;
    }

    void VulkanHighLevel::CreateRenderPass()
    {
        assert(m_pDevice && "Cannot create render pass, cause logical device is not valid.");
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SwapChainImageFormat;
        colorAttachment.samples = msaaSamples;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = FindSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
        depthAttachment.samples = msaaSamples;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = m_SwapChainImageFormat;
        colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
        colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite; //vk::AccessFlagBits::eColorAttachmentRead

        std::array<vk::AttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};

        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        ;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        m_pRenderPass = m_pDevice->createRenderPass(renderPassInfo);
        assert(m_pRenderPass && "Render pass was not created");
    }

    void VulkanHighLevel::CleanupSwapChain()
    {
        m_pDevice->destroyImageView(m_DepthImageView);
        m_pDevice->destroyImage(m_DepthImage);
        m_pDevice->freeMemory(m_DepthImageMemory);

        m_pDevice->destroyImageView(m_MSAAImageView);
        m_pDevice->destroyImage(m_MSAAImage);
        m_pDevice->freeMemory(m_MSAAImageMemory);

        for (auto framebuffer : m_vSwapChainFrameBuffers)
        {
            m_pDevice->destroyFramebuffer(framebuffer);
        }

        m_pDevice->freeCommandBuffers(m_pCommandPool, m_vCommandBuffer);

        m_pDevice->destroyRenderPass(m_pRenderPass);

        for (auto imageView : m_vSwapChainImageViews)
        {
            m_pDevice->destroyImageView(imageView);
        }

        m_pDevice->destroySwapchainKHR(m_pSwapchain);
    }

    void VulkanHighLevel::RecreateSwapChain()
    {
        m_pDevice->waitIdle();

        CleanupSwapChain();

        CreateSwapChain();
        CreateSwapChainImageViews();
        CreateRenderPass();
        CreateMSAAResources();
        CreateDepthResources();
        CreateFrameBuffers();
        
        for(auto& pipeline : m_vPipelines)
        {
            pipeline->RecreatePipeline(m_pDevice, m_DescriptorSetLayout, m_pRenderPass, m_SwapChainExtent);
        }

        CreateCommandBuffers();
    }

    void VulkanHighLevel::ValidateRunAbility()
    {
        assert(m_pVkInstance && "Vulkan instance is invalid.");
        if(VulkanStaticHelper::m_bEnableValidationLayers)
        assert(m_pVkDebugUtils && "Debud layer activated but not valid");
        assert(m_pSurface && "Vulkan screenspace surface is not valid.");

        assert(m_PhysicalDevice && "Physical device is not valid");
        assert(m_pDevice && "Logical device is not valid");
        assert(m_qGraphicsQueue && "Logical device graphics queue is not valid");
        assert(m_qPresentQueue && "Logical device present queue is not valid");

        assert(m_pSwapchain && "Swapchain is not valid.");
        assert(!m_vSwapChainImages.empty() && "Swap chain images are empty");

        assert(!m_vSwapChainImageViews.empty() && "Swap chain image views are empty, but should be same size as images.");
        assert(m_pRenderPass && "Render pass is not valid");
        assert(!m_vSwapChainFrameBuffers.empty() && "Swap chain framebuffers are empty");

        assert(m_pCommandPool && "Command pool is not valid");
    }

    void VulkanHighLevel::CreateSwapChainImageViews()
    {
        m_vSwapChainImageViews.resize(m_vSwapChainImages.size());

        for (size_t i = 0; i < m_vSwapChainImages.size(); i++)
        {
            m_vSwapChainImageViews[i] = CreateImageView(m_vSwapChainImages[i], 1, m_SwapChainImageFormat, vk::ImageAspectFlagBits::eColor);
        }
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
        CreateOnDeviceBuffer(imgSize, vk::BufferUsageFlagBits::eTransferSrc,
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                             stagingBuffer,
                             stagingBufferMemory);

        MoveToMemory(raw_data, stagingBufferMemory , imgSize);

        stbi_image_free(raw_data);

        CreateImage(m_pTexture->image, m_pTexture->deviceMemory, compiledSize, m_pTexture->mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, 
                    vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
                    vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        TransitionImageLayout(m_pTexture->image, m_pTexture->mipLevels, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        CopyBufferToImage(stagingBuffer, m_pTexture->image, compiledSize);
        GenerateMipmaps(m_pTexture->image, m_pTexture->mipLevels, vk::Format::eR8G8B8A8Srgb, compiledSize, vk::ImageAspectFlagBits::eColor);

        m_pDevice->destroyBuffer(stagingBuffer);
        m_pDevice->freeMemory(stagingBufferMemory);

        m_pTexture->view = CreateImageView(m_pTexture->image, m_pTexture->mipLevels, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

        CreateSampler(m_pTexture->sampler, m_pTexture->mipLevels);
        m_pTexture->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        m_pTexture->UpdateDescriptor();
    }

    //TODO: Upgrade this method to make it more customizable
    void VulkanHighLevel::CreateSampler(vk::Sampler& sampler, uint32_t mip_levels)
    {
        assert(m_PhysicalDevice && "Cannot create sampler, cause physical device is not valid.");
        assert(m_pDevice && "Cannot create sampler, cause logical device is not valid.");
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

        vk::PhysicalDeviceProperties properties{};
        properties = m_PhysicalDevice.getProperties();

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;

        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mip_levels);

        //TODO: Result handle
        auto result = m_pDevice->createSampler(&samplerInfo, nullptr, &sampler);
        assert(sampler && "Texture sampler was not created");
    }

    void VulkanHighLevel::TransitionImageLayout(vk::Image& image, uint32_t mip_levels, vk::ImageAspectFlags aspectFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) 
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } 
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) 
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } 
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) 
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else 
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(
            sourceStage,
            destinationStage,
            vk::DependencyFlags(),
            0,nullptr, 0, nullptr, 1, &barrier);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanHighLevel::CreateImage(vk::Image& image, vk::DeviceMemory& memory, vk::Extent3D size, uint32_t mip_levels, vk::SampleCountFlagBits num_samples, 
                                      vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        assert(m_pDevice && "Cannot create image, cause logical device is not valid.");
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = static_cast<uint32_t>(size.width);
        imageInfo.extent.height = static_cast<uint32_t>(size.height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mip_levels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = usage;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;
        imageInfo.samples = num_samples;

        //TODO: Check valid
        image = m_pDevice->createImage(imageInfo, nullptr);
        assert(image && "Image was not created");

        vk::MemoryRequirements memReq{};
        m_pDevice->getImageMemoryRequirements(image, &memReq);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);

        memory = m_pDevice->allocateMemory(allocInfo);
        assert(memory && "Image memory was not allocated");

        m_pDevice->bindImageMemory(image, memory, 0);
    }

    void VulkanHighLevel::CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, vk::Extent3D sizes)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D
        {
            static_cast<uint32_t>(sizes.width),
            static_cast<uint32_t>(sizes.height),
            1
        };

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        EndSingleTimeCommands(commandBuffer);
    }

    vk::ImageView VulkanHighLevel::CreateImageView(vk::Image& pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags)
    {
        assert(m_pDevice && "Cannot create image view, cause logical device is not valid.");
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = pImage;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = eFormat;
        viewInfo.subresourceRange.aspectMask = aspectFlags;;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mip_levels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView;
        //TODO: Handle result
        auto result = m_pDevice->createImageView(&viewInfo, nullptr, &imageView);
        assert(imageView && "Was not created");

        return imageView;
    }

    void VulkanHighLevel::GenerateMipmaps(vk::Image &image, uint32_t mipLevels, vk::Format format, vk::Extent3D sizes, vk::ImageAspectFlags aspectFlags)
    {
        assert(m_PhysicalDevice && "Cannot generate mipmaps, cause physical device is not valid.");
        assert(m_pDevice && "Cannot generate mipmaps, cause logical device is not valid.");
        vk::FormatProperties formatProperties;
        m_PhysicalDevice.getFormatProperties(format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) 
        {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = sizes.width;
        int32_t mipHeight = sizes.height;

        for (uint32_t i = 1; i < mipLevels; i++) 
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, 
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            vk::ImageBlit blit{};
            blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = aspectFlags;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
            blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = aspectFlags;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            commandBuffer.blitImage(
                image, vk::ImageLayout::eTransferSrcOptimal,
                image, vk::ImageLayout::eTransferDstOptimal,
                1, &blit,
                vk::Filter::eLinear);

            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &barrier);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanHighLevel::CreateDepthResources()
    {
        vk::Format depthFormat = FindSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        CreateImage(m_DepthImage, m_DepthImageMemory, vk::Extent3D{m_SwapChainExtent, 0}, 1,
        msaaSamples, depthFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eDepthStencilAttachment,  vk::MemoryPropertyFlagBits::eDeviceLocal);

        m_DepthImageView = CreateImageView(m_DepthImage, 1, depthFormat, vk::ImageAspectFlagBits::eDepth);

        TransitionImageLayout(m_DepthImage, 1, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    void VulkanHighLevel::CreateMSAAResources()
    {
        vk::Format colorFormat = m_SwapChainImageFormat;

        CreateImage(m_MSAAImage, m_MSAAImageMemory, vk::Extent3D{m_SwapChainExtent, 0}, 1, 
        msaaSamples, colorFormat, vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, 
        vk::MemoryPropertyFlagBits::eDeviceLocal);

        m_MSAAImageView = CreateImageView(m_MSAAImage, 1, colorFormat, vk::ImageAspectFlagBits::eColor);
    }

    vk::Format VulkanHighLevel::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;
            m_PhysicalDevice.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        //TODO: Handle null result
        throw std::runtime_error("Failed to find supported format!");
    }

    void VulkanHighLevel::CreateSyncObjects()
    {
        m_vImageAvailableSemaphores.resize(m_iFramesInFlight);
        m_vRenderFinishedSemaphores.resize(m_iFramesInFlight);
        m_vInFlightFences.resize(m_iFramesInFlight);

        try
        {
            for (size_t i = 0; i < m_iFramesInFlight; i++)
            {
                m_vImageAvailableSemaphores[i] = m_pDevice->createSemaphore({});
                m_vRenderFinishedSemaphores[i] = m_pDevice->createSemaphore({});
                m_vInFlightFences[i] = m_pDevice->createFence({vk::FenceCreateFlagBits::eSignaled});
            }
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }

    void VulkanHighLevel::DrawFrame(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
        //TODO: Add result handles
        m_pDevice->waitForFences(1, &m_vInFlightFences[m_iCurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        uint32_t imageIndex;
        try
        {
            vk::Result result = m_pDevice->acquireNextImageKHR(m_pSwapchain, std::numeric_limits<uint64_t>::max(),
                                                                 m_vImageAvailableSemaphores[m_iCurrentFrame], nullptr, &imageIndex);
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

        vk::SubmitInfo submitInfo = {};

        vk::Semaphore waitSemaphores[] = {m_vImageAvailableSemaphores[m_iCurrentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_vCommandBuffer[imageIndex];

        vk::Semaphore signalSemaphores[] = {m_vRenderFinishedSemaphores[m_iCurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        //TODO: Handle this result
        m_pDevice->resetFences(1, &m_vInFlightFences[m_iCurrentFrame]);

        try
        {
            m_qGraphicsQueue.submit(submitInfo, m_vInFlightFences[m_iCurrentFrame]);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        vk::SwapchainKHR swapChains[] = {m_pSwapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vk::Result resultPresent;
        try
        {
            resultPresent = m_qPresentQueue.presentKHR(presentInfo);
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

        m_iCurrentFrame = (m_iCurrentFrame + 1) % m_iFramesInFlight;
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
            m_pDevice->destroyBuffer(mesh.vertexBuffer);
            m_pDevice->freeMemory(mesh.vertexBufferMemory);

            mesh.indices.clear();
            m_pDevice->destroyBuffer(mesh.indexBuffer);
            m_pDevice->freeMemory(mesh.indiciesBufferMemory);
        }
        m_vMeshes.clear();

        for (size_t i = 0; i < m_iFramesInFlight; i++)
        {
            m_pDevice->destroySemaphore(m_vRenderFinishedSemaphores[i]);
            m_pDevice->destroySemaphore(m_vImageAvailableSemaphores[i]);
            m_pDevice->destroyFence(m_vInFlightFences[i]);
        }

        m_pDevice->destroyCommandPool(m_pCommandPool);

        // surface is created by glfw, therefore not using a Unique handle
        m_pVkInstance->destroySurfaceKHR(m_pSurface);

        if (VulkanStaticHelper::m_bEnableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(m_pVkInstance.get(), m_pVkDebugUtils, nullptr);
        }

        //Engine::PEngine()->PWindow()->Close();
    }
}