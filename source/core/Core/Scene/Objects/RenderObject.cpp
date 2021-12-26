#include "RenderObject.h"
#include "Core/VulkanDevice.h"

using namespace Engine::Core::Scene::Objects;
using namespace Engine::Resources;

RenderObject::RenderObject()
{
}

RenderObject::RenderObject(std::string srName)
{
    m_srName = srName;
}

void RenderObject::Create()
{
    m_pThis = shared_from_this();
}

void RenderObject::Create(std::shared_ptr<ResourceManager> resourceMgr)
{
    Create();
    for (auto &[name, child] : m_mChilds)
        child->Create(resourceMgr);
}

void RenderObject::ReCreate()
{
    for (auto &[name, child] : m_mChilds)
        child->ReCreate();
}

void RenderObject::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    for (auto &[name, child] : m_mChilds)
        child->Render(commandBuffer, imageIndex);
}

void RenderObject::Update(float fDeltaTime)
{
    for (auto &[name, child] : m_mChilds)
        child->Update(fDeltaTime);
}

void RenderObject::Cleanup()
{
    for (auto &[name, child] : m_mChilds)
        child->Cleanup();
}

void RenderObject::Destroy()
{
    for (auto &[name, child] : m_mChilds)
        child->Destroy();
}