#include "ResourceBase.h"
#include "ResourceManager.h"

using namespace Engine::Resources;
void ResourceBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
}

void ResourceBase::ReCreate()
{
}

void ResourceBase::Update(uint32_t imageIndex)
{
}

void ResourceBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
}

void ResourceBase::Cleanup()
{
}

void ResourceBase::Destroy()
{
}