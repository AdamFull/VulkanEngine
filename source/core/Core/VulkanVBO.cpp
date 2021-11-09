#include "VulkanVBO.h"
#include "VulkanAllocator.h"

using namespace Engine::Core;

VulkanVBO::VulkanVBO(std::shared_ptr<Device> device) : m_device(device)
{

}

VulkanVBO::~VulkanVBO()
{

}

void VulkanVBO::Create()
{
    CreateVertexBuffer();
    CreateIndexBuffer();
    m_bBuffersCreated = true;
}

void VulkanVBO::Bind(vk::CommandBuffer commandBuffer)
{
    assert(m_bBuffersCreated && "Vertex and index buffers are not created!");
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer->GetBuffer(), offsets);
    commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
}

void VulkanVBO::AddMeshData(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices)
{
    m_vVertices.insert(m_vVertices.end(), vertices.begin(), vertices.end());
    m_vIndices.insert(m_vIndices.end(), indices.begin(), indices.end());
}

void VulkanVBO::CreateVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(m_vVertices[0]) * m_vVertices.size();
    uint32_t vertexSize = sizeof(m_vVertices[0]);

    VulkanBuffer stagingBuffer(m_device);
    stagingBuffer.Create(vertexSize, m_vVertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.MapMem();
    stagingBuffer.Write((void *)m_vVertices.data());

    vertexBuffer = FDefaultAllocator::Allocate<VulkanBuffer>();
    vertexBuffer->Create(vertexSize, m_vVertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_device->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
    // m_vVertices.clear();
}

void VulkanVBO::CreateIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(m_vIndices[0]) * m_vIndices.size();
    uint32_t indexSize = sizeof(m_vIndices[0]);

    VulkanBuffer stagingBuffer(m_device);
    stagingBuffer.Create(indexSize, m_vIndices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.MapMem();
    stagingBuffer.Write((void *)m_vIndices.data());

    indexBuffer = FDefaultAllocator::Allocate<VulkanBuffer>();
    indexBuffer->Create(indexSize, m_vIndices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_device->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
    // m_vIndices.clear();
}