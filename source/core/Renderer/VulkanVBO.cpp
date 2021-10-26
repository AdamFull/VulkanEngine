#include "VulkanVBO.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"

namespace Engine
{
    void VulkanVBO::Create(std::unique_ptr<Device>& device)
    {
        CreateVertexBuffer(device);
        CreateIndexBuffer(device);
        m_bBuffersCreated = true;
    }

    void VulkanVBO::Bind(vk::CommandBuffer commandBuffer)
    {
        assert(m_bBuffersCreated && "Vertex and index buffers are not created!");
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer->GetBuffer(), offsets);
        commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
    }

    void VulkanVBO::Destroy(std::unique_ptr<Device>& device)
    {
        vertexBuffer->Destroy(device);
        indexBuffer->Destroy(device);
    }

    void VulkanVBO::AddMeshData(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
    {
        m_vVertices.insert(m_vVertices.end(), vertices.begin(), vertices.end());
        m_vIndices.insert(m_vIndices.end(), indices.begin(), indices.end());
    }

    void VulkanVBO::CreateVertexBuffer(std::unique_ptr<Device>& device)
    {
        vk::DeviceSize bufferSize = sizeof(m_vVertices[0]) * m_vVertices.size();
        uint32_t vertexSize = sizeof(m_vVertices[0]);

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(device, vertexSize, m_vVertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(device);
        stagingBuffer.Write(device, (void*)m_vVertices.data());

        vertexBuffer = std::make_unique<VulkanBuffer>();
        vertexBuffer->Create(device, vertexSize, m_vVertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
        device->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

        stagingBuffer.Destroy(device);
        //m_vVertices.clear();
    }

    void VulkanVBO::CreateIndexBuffer(std::unique_ptr<Device>& device)
    {
        vk::DeviceSize bufferSize = sizeof(m_vIndices[0]) * m_vIndices.size();
        uint32_t indexSize = sizeof(m_vIndices[0]);

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(device, indexSize, m_vIndices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(device);
        stagingBuffer.Write(device, (void*)m_vIndices.data());

        indexBuffer = std::make_unique<VulkanBuffer>();
        indexBuffer->Create(device, indexSize, m_vIndices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
        device->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

        stagingBuffer.Destroy(device);
        //m_vIndices.clear();
    }
}