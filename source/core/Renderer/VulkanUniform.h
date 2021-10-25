#pragma once
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

namespace Engine
{
    struct FUniformData 
    {
        glm::mat4 model{};
        glm::mat4 view{};
        glm::mat4 projection{};
        glm::mat4 normal{};

        glm::vec4 lightPosition{};
        glm::vec4 viewPosition{};
    };

    struct FLightData
    {
        glm::vec4 viewPosition{};
        glm::vec4 lightPosition{};
        glm::vec4 lightColor{};
    };

    struct FUniformDataUI 
    {
		glm::vec2 scale;
		glm::vec2 translate;
	};

    template<class uboType>
    class UniformBuffer
    {
    public:
        UniformBuffer() = default;
        UniformBuffer(const UniformBuffer&) = delete;
        void operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        void Create(std::unique_ptr<Device>& device)
        {
            CreateUniformBuffers(device);
        }

        void ReCreate(std::unique_ptr<Device>& device)
        {
            Create(device);
        }

        void Cleanup(std::unique_ptr<Device>& device)
        {
            m_pBuffer->Destroy(device);
        }

        void UpdateUniformBuffer(std::unique_ptr<Device>& device, uboType ubo)
        {
            m_pBuffer->Write(device, &ubo);
            m_pBuffer->Flush(device);
        }

        // Getters
        inline std::unique_ptr<VulkanBuffer>& GetUniformBuffer() { return m_pBuffer; }
    private:
        void CreateUniformBuffers(std::unique_ptr<Device>& device)
        {

            auto physProps = device->GetPhysical().getProperties();
            auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
            m_pBuffer = std::make_unique<VulkanBuffer>();
            m_pBuffer->Create(device, sizeof(uboType), 1, vk::BufferUsageFlagBits::eUniformBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
            m_pBuffer->MapMem(device);
        }

        std::unique_ptr<VulkanBuffer> m_pBuffer;
    };
}