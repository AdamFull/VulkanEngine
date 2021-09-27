#include "StaticMesh.h"
#include "DataTypes/VulkanMesh.h"

namespace Engine
{
    void StaticMesh::Create(std::unique_ptr<Device>& device)
    {
        RenderObject::Create(device);
        m_pStaticMesh = std::make_unique<VulkanStaticMesh>();
    }

    void StaticMesh::Update(float fDeltaTime)
    {
        RenderObject::Update(fDeltaTime);
    }

    void StaticMesh::Render(float fDeltaTime, vk::CommandBuffer& commandBuffer)
    {
        RenderObject::Render(fDeltaTime, commandBuffer);
        m_pStaticMesh->Bind(commandBuffer);
        m_pStaticMesh->Draw(commandBuffer);
    }
}