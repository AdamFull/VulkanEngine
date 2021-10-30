#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

using namespace Engine::Core;

void UniformBuffer::Create(std::unique_ptr<Device> &device, uint32_t inFlightFrames, size_t uniform_size)
{
    m_iUniformSize = uniform_size;
    CreateUniformBuffers(device, inFlightFrames);
}

void UniformBuffer::ReCreate(std::unique_ptr<Device> &device, uint32_t inFlightFrames)
{
    Create(device, inFlightFrames, m_iUniformSize);
}

void UniformBuffer::Cleanup(std::unique_ptr<Device> &device)
{
    for (auto &buffer : m_pBuffers)
        buffer->Destroy(device);
    m_pBuffers.clear();
}
void UniformBuffer::UpdateUniformBuffer(std::unique_ptr<Device> &device, uint32_t index, void *ubo)
{
    m_pBuffers[index]->Write(device, ubo);
    m_pBuffers[index]->Flush(device);
}

void UniformBuffer::CreateUniformBuffers(std::unique_ptr<Device> &device, uint32_t inFlightFrames)
{
    m_pBuffers.resize(inFlightFrames);

    auto physProps = device->GetPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = std::make_unique<VulkanBuffer>();
        uniform->Create(device, m_iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        uniform->MapMem(device);

        m_pBuffers[i] = std::move(uniform);
    }
}

std::vector<std::unique_ptr<VulkanBuffer>> &UniformBuffer::GetUniformBuffers()
{
    return m_pBuffers;
}

std::unique_ptr<VulkanBuffer> &UniformBuffer::GetUniformBuffer(uint32_t index)
{
    return m_pBuffers[index];
}