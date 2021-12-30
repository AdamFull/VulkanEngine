#include "RenderPass.hpp"
#include "Core/VulkanHighLevel.h"

using namespace Core::Render;

void CRenderPass::Create(const CRenderStage &renderStage, vk::Format depthFormat, vk::Format surfaceFormat, vk::SampleCountFlagBits samples)
{

}

CRenderPass::~CRenderPass()
{
    if(renderPass)
    {
        UDevice->Destroy(renderPass);
    }
}