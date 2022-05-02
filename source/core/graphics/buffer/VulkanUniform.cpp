#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CUniformBuffer::~CUniformBuffer()
{

}

void CUniformBuffer::create(uint32_t inFlightFrames, size_t uniform_size)
{
    iUniformSize = uniform_size;
    createUniformBuffers(inFlightFrames);
}

void CUniformBuffer::reCreate(uint32_t inFlightFrames)
{
    create(inFlightFrames, iUniformSize);
}

void CUniformBuffer::cleanup()
{
    vBuffers.clear();
}
void CUniformBuffer::updateUniformBuffer(uint32_t index, void *ubo)
{
    vBuffers[index]->Write(ubo);
    vBuffers[index]->Flush();
}

void CUniformBuffer::createUniformBuffers(uint32_t inFlightFrames)
{
    vBuffers.resize(inFlightFrames);

    auto physProps = UDevice->GetPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = std::make_unique<VulkanBuffer>();
        uniform->Create(iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        uniform->MapMem();

        vBuffers[i] = std::move(uniform);
    }
}

std::vector<std::unique_ptr<VulkanBuffer>> &CUniformBuffer::getUniformBuffers()
{
    return vBuffers;
}

std::unique_ptr<VulkanBuffer> &CUniformBuffer::getUniformBuffer(uint32_t index)
{
    return vBuffers[index];
}