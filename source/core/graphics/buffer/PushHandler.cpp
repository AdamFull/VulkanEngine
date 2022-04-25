#include "PushHandler.hpp"
#include "graphics/VulkanHighLevel.h"
#include "graphics/buffer/VulkanBuffer.h"

using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

void PushHandler::Create(const UniformBlock &uniformBlock)
{
    uint32_t images = USwapChain->GetFramesInFlight();
    for(auto i = 0; i < images; i++)
        m_vData.emplace_back(std::make_unique<char[]>(uniformBlock.GetSize()));
    m_uniformBlock = uniformBlock;
}

void PushHandler::ReCreate()
{
    Create(m_uniformBlock.value());
}

void PushHandler::Cleanup()
{
    m_vData.clear();
}

void PushHandler::Flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::PipelineBase> pPipeline)
{
    uint32_t index = USwapChain->GetCurrentFrame();
    if (!m_vData.empty())
		return;
    
    auto& data = m_vData.at(index);
    if(data)
    {
        commandBuffer.pushConstants(pPipeline->GetPipelineLayout(), m_uniformBlock->GetStageFlags(), 0, static_cast<uint32_t>(m_uniformBlock->GetSize()), data.get());
    }
}
