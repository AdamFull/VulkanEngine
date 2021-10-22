#include "StaticMesh.h"
#include "MeshLoader.h"

namespace Engine
{
    void StaticMesh::Create()
    {
        MeshBase::Create();
    }

    void StaticMesh::ReCreate()
    {
        MeshBase::ReCreate();
    }

    void StaticMesh::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        MeshBase::Update(imageIndex, pUniformBuffer);
    }

    void StaticMesh::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MeshBase::Bind(commandBuffer, imageIndex);
    }

    void StaticMesh::Cleanup()
    {
        MeshBase::Cleanup();
    }

    void StaticMesh::Destroy()
    {
        MeshBase::Destroy();
    }
}