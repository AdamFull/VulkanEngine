#include "UniformHandler.hpp"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void CUniformHandler::create(const CUniformBlock &_uniformBlock)
{
    CHandler::create(_uniformBlock);

    auto physProps = CDevice::inst()->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    
    auto uniform = std::make_unique<CVulkanBuffer>();
    uniform->create(uniformBlock->getSize(), 1, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
    uniform->mapMem();
    pBuffer = std::move(uniform);
}