#include "UniformHandler.hpp"
#include "VulkanBuffer.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

CUniformHandler::~CUniformHandler()
{
    cleanup();
}

void CUniformHandler::create(const CUniformBlock &_uniformBlock)
{
    uint32_t images = CDevice::inst()->getFramesInFlight();
    vMapped.resize(images);
    iUniformSize = _uniformBlock.getSize();
    uniformBlock = _uniformBlock;
    createUniformBuffers(images);
}

void CUniformHandler::reCreate()
{
    create(uniformBlock.value());
}

void CUniformHandler::cleanup()
{
    if(!bIsClean)
    {
        for(auto& buffer : vBuffers)
            buffer->cleanup();
        vBuffers.clear();
        vMapped.clear();
        bIsClean = true;
    }
}

void CUniformHandler::flush()
{
    uint32_t index = CDevice::inst()->getCurrentFrame();
    if (vBuffers.empty())
		return;
    
    if(vMapped.at(index))
    {
        vBuffers.at(index)->flush();
        //vBuffers.at(index)->unmapMem();
        vMapped.at(index) = false;
    }
}

void CUniformHandler::createUniformBuffers(uint32_t inFlightFrames)
{
    vBuffers.resize(inFlightFrames);

    auto physProps = CDevice::inst()->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = std::make_unique<CVulkanBuffer>();
        uniform->create(iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        uniform->mapMem();

        vBuffers[i] = std::move(uniform);
    }
}

std::vector<std::unique_ptr<CVulkanBuffer>> &CUniformHandler::getUniformBuffers()
{
    return vBuffers;
}

std::unique_ptr<CVulkanBuffer> &CUniformHandler::getUniformBuffer(uint32_t index)
{
    return vBuffers.at(index);
}