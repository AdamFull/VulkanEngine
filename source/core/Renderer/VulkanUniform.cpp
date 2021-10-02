#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

namespace Engine
{
    void UniformBuffer::Create(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
    {
        CreateUniformBuffers(device, inFlightFrames);
    }

    void UniformBuffer::ReCreate(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
    {
        Create(device, inFlightFrames);
    }

    void UniformBuffer::Cleanup(std::unique_ptr<Device>& device)
    {
        for(auto& buffer : m_pBuffers)
            buffer->Destroy(device);
        m_pBuffers.clear();
    }

    void UniformBuffer::Bind(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& layout, vk::DescriptorSet descriptorSet)
    {
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, 1, &descriptorSet, 0, nullptr);
    }

    void UniformBuffer::CreateUniformBuffers(std::unique_ptr<Device>& device, uint32_t inFlightFrames)
    {
        m_pBuffers.resize(inFlightFrames);

        auto physProps = device->GetPhysical().getProperties();
        auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
        for (size_t i = 0; i < inFlightFrames; i++) 
        {
            auto uniform = std::make_unique<VulkanBuffer>();
            uniform->Create(device, sizeof(FUniformData), 1, vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
            uniform->MapMem(device);

            m_pBuffers[i] = std::move(uniform);
        }
    }

    void UniformBuffer::UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, glm::mat4 transform, glm::vec3 light_pos)
    {
        FUniformData ubo{};
        ubo.transform = transform;

        m_pBuffers[index]->Write(device, &ubo);
        m_pBuffers[index]->Flush(device);
    }
}