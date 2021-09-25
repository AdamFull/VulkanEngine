#pragma once
#include "DataTypes/VulkanVertex.h"
#include "DataTypes/VulkanQueueFamilies.h"
#include "DataTypes/VulkanSwapChainSipportDetails.h"
#include "DataTypes/VulkanTexture.h"
#include "DataTypes/VulkanMesh.h"

namespace Engine
{
    class WindowHandle;

    struct FShaderCache
    {
        vk::ShaderStageFlagBits sShaderType;
        std::vector<char> srShaderData;
    };

    VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
    void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    class VulkanHighLevel
    {
    public:
        VulkanHighLevel() = default;
        ~VulkanHighLevel();

        void Create(std::unique_ptr<WindowHandle>& pWindow, const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        void CreateInstance(const char *pApplicationName, uint32_t applicationVersion,
                                         const char *pEngineName, uint32_t engineVersion,
                                         uint32_t apiVersion);

        void CreateDescriptorSetLayout();

        //Pipeline
        void LoadShader(const std::string& srShaderPath, vk::ShaderStageFlagBits fShaderType);
        void LoadShader(const std::map<vk::ShaderStageFlagBits, std::string>& mShaders);

        void RecreateShaders();
        void LoadShader(const std::vector<char>& vShaderCode, vk::ShaderStageFlagBits fShaderType);
        void CreateGraphicsPipeline();

        void CreateFrameBuffers();
        void CreateCommandPool();

        void CreateDepthResources();
        vk::Format FindSupportedFormat(const std::vector<vk::Format>&, vk::ImageTiling, vk::FormatFeatureFlags);

        void CreateMSAAResources();

        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        template<class T>
        void MoveToMemory(T raw_data, vk::DeviceMemory& memory, vk::DeviceSize size)
        {
            void *data = m_pDevice->mapMemory(memory, 0, size);
            memcpy(data, raw_data, static_cast<size_t>(size));
            m_pDevice->unmapMemory(memory);
        }

        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> 
        CreateCommandBuffer(vk::CommandBufferLevel level, vk::CommandPool& pool, uint32_t count);
        void CreateCommandBuffers();
        vk::CommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(vk::CommandBuffer);
        
        void CreateSyncObjects();

        void AddVulkanTexture(std::string, uint32_t);

        void CreateVertexBuffer(std::vector<Vertex>& vertices, vk::Buffer& vertexBuffer, vk::DeviceMemory& vertexBufferMemory);
        void CreateIndexBuffer(std::vector<uint32_t>& indices, vk::Buffer& indexBuffer, vk::DeviceMemory& indiciesBufferMemory);
        void AddVulkanMesh(std::string, VulkanTransform);

        inline void GPUWait() { m_pDevice->waitIdle(); }

        inline void SetFramesInFlight(uint32_t iValue) { m_iFramesInFlight = iValue; }

        void DrawFrame(float);

        uint32_t FindMemoryType(uint32_t, vk::MemoryPropertyFlags);
        QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice);
        vk::PhysicalDevice GetPhysicalDevice();
        std::vector<vk::PhysicalDevice> GetAvaliablePhysicalDevices();
        bool IsDeviceSuitable(const vk::PhysicalDevice&);
        void CreateOnDeviceBuffer(vk::DeviceSize, vk::BufferUsageFlags, vk::MemoryPropertyFlags, vk::Buffer&, vk::DeviceMemory&);
        void CopyOnDeviceBuffer(vk::Buffer, vk::Buffer, vk::DeviceSize);

        vk::SampleCountFlagBits GetMaxUsableSampleCount();
        
        SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice&);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR&);

        void CreateImage(vk::Image&, vk::DeviceMemory&, vk::Extent3D, uint32_t, vk::SampleCountFlagBits, vk::Format, vk::ImageTiling, vk::ImageUsageFlags, vk::MemoryPropertyFlags);
        void TransitionImageLayout(vk::Image&, uint32_t, vk::ImageAspectFlags, vk::ImageLayout, vk::ImageLayout);
        void CopyBufferToImage(vk::Buffer&, vk::Image&, vk::Extent3D);
        vk::ImageView CreateImageView(vk::Image&, uint32_t, vk::Format, vk::ImageAspectFlags);
        void GenerateMipmaps(vk::Image&, uint32_t, vk::Format, vk::Extent3D, vk::ImageAspectFlags);
        void CreateSampler(vk::Sampler&, uint32_t);

        void UpdateUniformBuffer(uint32_t);

        void CleanupSwapChain();
        //void CleanupTexture();
        void Cleanup();
        void RecreateSwapChain();

        void ValidateRunAbility();

    private:
        void CreateDebugCallback();
        void CreateSurface(std::unique_ptr<WindowHandle>& pWindow);
        void CreateDevice();
        void CreateSwapChain();
        void CreateSwapChainImageViews();
        void CreateRenderPass();

    private:
        vk::UniqueInstance m_pVkInstance;                       //Main vulkan instance
        //vk::UniqueDebugUtilsMessengerEXT m_pVkDebugUtils;       //Vulkan's debugger ulitities
        VkDebugUtilsMessengerEXT m_pVkDebugUtils;
        vk::SurfaceKHR m_pSurface;                              //Vulkan's drawing surface

        //Device specific parameters
        vk::PhysicalDevice m_PhysicalDevice;
        vk::UniqueDevice m_pDevice;
        vk::Queue m_qGraphicsQueue;
        vk::Queue m_qPresentQueue;

        vk::DescriptorSetLayout m_DescriptorSetLayout;
        vk::DescriptorPool m_DescriptorPool;
        std::vector<vk::DescriptorSet> m_vDescriptorSets;

        //Pipeline
        std::vector<FShaderCache> m_vShaderCache;
        std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderBuffer;
        vk::PipelineLayout m_PipelineLayout;
        vk::Pipeline m_GraphicsPipeline;

        vk::SwapchainKHR m_pSwapchain;
        std::vector<vk::Image> m_vSwapChainImages;
        vk::Format m_SwapChainImageFormat;
        vk::Extent2D m_SwapChainExtent;

        std::vector<vk::ImageView> m_vSwapChainImageViews;
        vk::RenderPass m_pRenderPass;
        std::vector<vk::Framebuffer> m_vSwapChainFrameBuffers;

        vk::CommandPool m_pCommandPool;

        std::vector<vk::Buffer> m_vUniformBuffers;
        std::vector<vk::DeviceMemory> m_vUniformBuffersMemory;

        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> m_vCommandBuffer;

        std::vector<vk::Semaphore> m_vImageAvailableSemaphores;
        std::vector<vk::Semaphore> m_vRenderFinishedSemaphores;
        std::vector<vk::Fence> m_vInFlightFences;

        glm::vec3 m_vecPosition{0.f, 0.f, 0.f};

        //Loaded texture
        std::unique_ptr<VulkanTextureBase> m_pTexture;
        std::vector<VulkanStaticMesh> m_vMeshes;

        //Depth buffer
        vk::Image m_DepthImage;
        vk::DeviceMemory m_DepthImageMemory;
        vk::ImageView m_DepthImageView;

        //MSAA buffer
        vk::Image m_MSAAImage;
        vk::DeviceMemory m_MSAAImageMemory;
        vk::ImageView m_MSAAImageView;

        vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;

        uint32_t m_iFramesInFlight{2};
        uint32_t m_iCurrentFrame{0};

        float m_fDeltaTime{0.0f};
    };
}
