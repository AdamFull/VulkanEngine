#include "StorageHandler.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::core::pipeline;

CStorageHandler::CStorageHandler(const pipeline::CUniformBlock &uniformBlock)
{
    create(uniformBlock);
}

void CStorageHandler::create(const CUniformBlock &_uniformBlock)
{
    CHandler::create(_uniformBlock);
    auto framesInFlight = UDevice->getFramesInFlight();
    
    auto physProps = UDevice->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    
    for(uint32_t i = 0; i < framesInFlight; i++)
    {
        auto uniform = utl::make_scope<CVulkanBuffer>(uniformBlock->getSize(), 1, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
        minOffsetAllignment);
        vBuffers.emplace_back(std::move(uniform));
    }
}