#include "StorageHandler.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void CStorageHandler::create(const CUniformBlock &_uniformBlock)
{
    CHandler::create(_uniformBlock);
    
    auto physProps = CDevice::inst()->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    
    auto uniform = std::make_unique<CVulkanBuffer>();
    uniform->create(uniformBlock->getSize(), 1, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);

    pBuffer = std::move(uniform);
}