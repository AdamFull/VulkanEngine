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
    vBuffers[index]->write(ubo);
    vBuffers[index]->flush();
}

void CUniformBuffer::createUniformBuffers(uint32_t inFlightFrames)
{
    vBuffers.resize(inFlightFrames);

    auto physProps = UDevice->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = std::make_unique<CVulkanBuffer>();
        uniform->create(iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        uniform->mapMem();

        vBuffers[i] = std::move(uniform);
    }
}

std::vector<std::unique_ptr<CVulkanBuffer>> &CUniformBuffer::getUniformBuffers()
{
    return vBuffers;
}

std::unique_ptr<CVulkanBuffer> &CUniformBuffer::getUniformBuffer(uint32_t index)
{
    return vBuffers[index];
}