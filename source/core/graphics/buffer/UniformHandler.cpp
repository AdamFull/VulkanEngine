#include "UniformHandler.hpp"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void CUniformHandler::create(const CUniformBlock &_uniformBlock)
{
    CHandler::create(_uniformBlock);
    auto framesInFlight = CDevice::inst()->getFramesInFlight();

    auto physProps = CDevice::inst()->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    
    for(uint32_t i = 0; i < framesInFlight; i++)
    {
        auto uniform = make_scope<CVulkanBuffer>();
        uniform->create(uniformBlock->getSize(), 1, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        vBuffers.emplace_back(std::move(uniform));
    }
}