#include "UniformHandler.hpp"
#include "VulkanBuffer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void UniformHandler::Create(const UniformBlock &uniformBlock)
{
    uint32_t images = USwapChain->GetFramesInFlight();
    m_vMapped.resize(images);
    m_iUniformSize = uniformBlock.GetSize();
    m_uniformBlock = uniformBlock;
    CreateUniformBuffers(images);
}

void UniformHandler::ReCreate()
{
    Create(m_uniformBlock.value());
}

void UniformHandler::Cleanup()
{
    m_pBuffers.clear();
    m_vMapped.clear();
}

void UniformHandler::Flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::PipelineBase> pPipeline)
{
    uint32_t index = USwapChain->GetCurrentFrame();
    if (!m_pBuffers.empty())
		return;
    
    if(m_vMapped.at(index))
    {
        m_pBuffers.at(index)->Flush();
        m_pBuffers.at(index)->UnmapMem();
        m_vMapped.at(index) = false;
    }
}

void UniformHandler::CreateUniformBuffers(uint32_t inFlightFrames)
{
    m_pBuffers.resize(inFlightFrames);

    auto physProps = UDevice->GetPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    for (size_t i = 0; i < inFlightFrames; i++)
    {
        auto uniform = std::make_unique<VulkanBuffer>();
        uniform->Create(m_iUniformSize, 1, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, minOffsetAllignment);
        //uniform->MapMem();

        m_pBuffers[i] = std::move(uniform);
    }
}

std::vector<std::unique_ptr<VulkanBuffer>> &UniformHandler::GetUniformBuffers()
{
    return m_pBuffers;
}

std::unique_ptr<VulkanBuffer> &UniformHandler::GetUniformBuffer(uint32_t index)
{
    return m_pBuffers.at(index);
}