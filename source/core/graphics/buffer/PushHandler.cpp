#include "PushHandler.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/buffer/VulkanBuffer.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void PushHandler::Create(const CPushConstBlock &uniformBlock)
{
    uint32_t images = USwapChain->GetFramesInFlight();
    for(auto i = 0; i < images; i++)
        m_vData.emplace_back(std::make_unique<char[]>(uniformBlock.getSize()));
    m_pushBlock = uniformBlock;
}

void PushHandler::ReCreate()
{
    Create(m_pushBlock.value());
}

void PushHandler::Cleanup()
{
    m_vData.clear();
}

void PushHandler::Flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::CPipelineBase> pPipeline)
{
    uint32_t index = USwapChain->GetCurrentFrame();
    
    auto& data = m_vData.at(index);
    if(data)
    {
        commandBuffer.pushConstants(pPipeline->getPipelineLayout(), m_pushBlock->getStageFlags(), 0, static_cast<uint32_t>(m_pushBlock->getSize()), data.get());
    }
}
