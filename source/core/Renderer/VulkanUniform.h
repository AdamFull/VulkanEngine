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

        void Create(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
        {
            CreateUniformBuffers(device, inFlightFrames);
        }

        void ReCreate(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
        {
            Create(device, inFlightFrames);
        }

        void Cleanup(std::unique_ptr<Device>& device)
        {
            for(auto& buffer : m_pBuffers)
                buffer->Destroy(device);
            m_pBuffers.clear();
        }

        void UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, uboType ubo)
        {
            m_pBuffers[index]->Write(device, &ubo);
            m_pBuffers[index]->Flush(device);
        }

        // Getters
        inline std::vector<std::unique_ptr<VulkanBuffer>>& GetUniformBuffers() { return m_pBuffers; }
        inline std::unique_ptr<VulkanBuffer>& GetUniformBuffer(uint32_t index) { return m_pBuffers[index]; }
    private:
        void CreateUniformBuffers(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
        {
            m_pBuffers.resize(inFlightFrames);

            auto physProps = device->GetPhysical().getProperties();
            auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
            for (size_t i = 0; i < inFlightFrames; i++)
            {
                auto uniform = std::make_unique<VulkanBuffer>();
                uniform->Create(device, sizeof(uboType), 1, vk::BufferUsageFlagBits::eUniformBuffer,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
                uniform->MapMem(device);

                m_pBuffers[i] = std::move(uniform);
            }
        }

        std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
    };
}