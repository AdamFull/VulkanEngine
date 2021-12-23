#include "VulkanUniform.h"
#include "VulkanDevice.h"
#include "VulkanAllocator.h"

using namespace Engine::Core;

UniformBuffer::UniformBuffer(std::shared_ptr<Device> device) : m_device(device)
{

}

UniformBuffer::~UniformBuffer()
{

}

void UniformBuffer::Create(uint32_t inFlightFrames, size_t uniform_size)
{
    m_iUniformSize = uniform_size;
    CreateUniformBuffers(inFlightFrames);
}

void UniformBuffer::ReCreate(uint32_t inFlightFrames)
{
    Create(inFlightFrames, m_iUniformSize);
}

void UniformBuffer::Cleanup()
{
    m_pBuffers.clear();
}
void UniformBuffer::UpdateUniformBuffer(uint32_t index, void *ubo)
{
    m_pBuffers[index]->Write(ubo);
    m_pBuffers[index]->Flush();
}

void UniformBuffer::CreateUniformBuffers(uint32_t inFlightFrames)
{
    m_pBuffers.resize(inFlightFrames);

    auto physProps = m_device->GetPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = FDefaultAllocator::Allocate<VulkanBuffer>();
        uniform->Create(m_iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        uniform->MapMem();

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