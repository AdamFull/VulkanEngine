#include "VulkanDevice.h"
#include "WindowHandle.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace Engine::Device
{
    SwapChainSupportDetails QuerySwapChainSupport(Main::FVulkanRenderInstance& renderInstance)
    {
        SwapChainSupportDetails details;
        details.capabilities = renderInstance.physicalDevice.getSurfaceCapabilitiesKHR(renderInstance.surface);
        details.formats = renderInstance.physicalDevice.getSurfaceFormatsKHR(renderInstance.surface);
        details.presentModes = renderInstance.physicalDevice.getSurfacePresentModesKHR(renderInstance.surface);

        return details;
    }

    uint32_t FindMemoryType(Main::FVulkanRenderInstance& renderInstance, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = renderInstance.physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }
    
    QueueFamilyIndices FindQueueFamilies(Main::FVulkanRenderInstance& renderInstance)
    {
        QueueFamilyIndices indices;

        auto queueFamilies = renderInstance.physicalDevice.getQueueFamilyProperties();

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && renderInstance.physicalDevice.getSurfaceSupportKHR(i, renderInstance.surface))
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
    
    vk::Format FindSupportedFormat(Main::FVulkanRenderInstance& renderInstance, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;
            renderInstance.physicalDevice.getFormatProperties(format, &props);

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
    

    void CreateOnDeviceBuffer(Main::FVulkanRenderInstance& renderInstance, vk::DeviceSize size, vk::BufferUsageFlags buffer_usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = size;
        bufferInfo.usage = buffer_usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        try
        {
            buffer = renderInstance.logicalDevice->createBuffer(bufferInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to create buffer!");
        }

        vk::MemoryRequirements memRequirements = renderInstance.logicalDevice->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(renderInstance, memRequirements.memoryTypeBits, properties);

        try
        {
            bufferMemory = renderInstance.logicalDevice->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("Failed to allocate buffer memory!");
        }

        renderInstance.logicalDevice->bindBufferMemory(buffer, bufferMemory, 0);
    }
    
    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>>
    CreateCommandBuffer(Main::FVulkanRenderInstance& renderInstance, vk::CommandBufferLevel level, vk::CommandPool &pool, uint32_t count)
    {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool = pool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = count;

        //TODO: Handle error
        return renderInstance.logicalDevice->allocateCommandBuffers(allocInfo);
    }
    
    vk::CommandBuffer BeginSingleTimeCommands(Main::FVulkanRenderInstance& renderInstance)
    {
        vk::CommandBuffer commandBuffer = CreateCommandBuffer(renderInstance, vk::CommandBufferLevel::ePrimary, renderInstance.commandPool, 1)[0];

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }
    
    void EndSingleTimeCommands(Main::FVulkanRenderInstance& renderInstance, vk::CommandBuffer &commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        renderInstance.graphicsQueue.submit(submitInfo, nullptr);
        renderInstance.graphicsQueue.waitIdle();

        renderInstance.logicalDevice->freeCommandBuffers(renderInstance.commandPool, commandBuffer);
    }
    
    void CopyOnDeviceBuffer(Main::FVulkanRenderInstance& renderInstance, vk::Buffer &srcBuffer, vk::Buffer &dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(renderInstance);

        vk::BufferCopy copyRegion = {};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        EndSingleTimeCommands(renderInstance, commandBuffer);
    }

    void CreateImage(Main::FVulkanRenderInstance& renderInstance, vk::Image &image, vk::DeviceMemory &memory, uint32_t width, uint32_t height, uint32_t mip_levels,
                     vk::SampleCountFlagBits num_samples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
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
        image = renderInstance.logicalDevice->createImage(imageInfo, nullptr);

        vk::MemoryRequirements memReq{};
        renderInstance.logicalDevice->getImageMemoryRequirements(image, &memReq);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(renderInstance, memReq.memoryTypeBits, properties);

        memory = renderInstance.logicalDevice->allocateMemory(allocInfo);
        renderInstance.logicalDevice->bindImageMemory(image, memory, 0);
    }

    void TransitionImageLayout(Main::FVulkanRenderInstance& renderInstance, vk::Image& image, uint32_t mip_levels, vk::ImageAspectFlags aspectFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(renderInstance);

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

        EndSingleTimeCommands(renderInstance, commandBuffer);
    }

    void CopyBufferToImage(Main::FVulkanRenderInstance& renderInstance, vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height, uint32_t layerCount)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(renderInstance);

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
            width,
            height,
            1
        };

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        EndSingleTimeCommands(renderInstance, commandBuffer);
    }

    vk::ImageView CreateImageView(Main::FVulkanRenderInstance& renderInstance, vk::Image &pImage, uint32_t mip_levels, vk::Format eFormat, vk::ImageAspectFlags aspectFlags)
    {
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
        auto result = renderInstance.logicalDevice->createImageView(&viewInfo, nullptr, &imageView);

        return imageView;
    }

    void GenerateMipmaps(Main::FVulkanRenderInstance& renderInstance, vk::Image &image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags)
    {
        vk::FormatProperties formatProperties;
        renderInstance.physicalDevice.getFormatProperties(format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) 
        {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(renderInstance);

        vk::ImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = width;
        int32_t mipHeight = height;

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

        EndSingleTimeCommands(renderInstance, commandBuffer);
    }

    void CreateSampler(Main::FVulkanRenderInstance& renderInstance, vk::Sampler& sampler, uint32_t mip_levels)
    {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

        vk::PhysicalDeviceProperties properties{};
        properties = renderInstance.physicalDevice.getProperties();

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
        auto result = renderInstance.logicalDevice->createSampler(&samplerInfo, nullptr, &sampler);
    }
    
    // helper functions
    bool IsDeviceSuitable(Main::FVulkanRenderInstance& renderInstance)
    {
        QueueFamilyIndices indices = FindQueueFamilies(renderInstance);

        bool extensionsSupported = CheckDeviceExtensionSupport(renderInstance);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(renderInstance);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        vk::PhysicalDeviceFeatures supportedFeatures{};
        supportedFeatures = renderInstance.physicalDevice.getFeatures();

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.sampleRateShading;
    }
    
    std::vector<const char *> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        //TODO: move glfw to glfw window
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (bEnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    
    bool CheckValidationLayerSupport()
    {
        auto availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char *layerName : vValidationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                std::cout << layerProperties.layerName << std::endl;
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
    
    void PopulateDebugMessengerCreateInfo(Main::FVulkanRenderInstance& renderInstance, vk::DebugUtilsMessengerCreateInfoEXT &createInfo)
    {

    }
    
    bool CheckDeviceExtensionSupport(Main::FVulkanRenderInstance& renderInstance)
    {
        std::set<std::string> sRequiredExtensions(vDeviceExtensions.begin(), vDeviceExtensions.end());

        for (const auto &extension : renderInstance.physicalDevice.enumerateDeviceExtensionProperties())
        {
            sRequiredExtensions.erase(extension.extensionName);
        }

        return sRequiredExtensions.empty();
    }
    
}