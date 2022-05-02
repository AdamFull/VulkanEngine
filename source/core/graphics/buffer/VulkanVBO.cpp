#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;

CVulkanVBO::~CVulkanVBO()
{

}

void CVulkanVBO::create()
{
    createVertexBuffer();
    createIndexBuffer();
    bBuffersCreated = true;
}

void CVulkanVBO::bind(vk::CommandBuffer commandBuffer)
{
    assert(bBuffersCreated && "Vertex and index buffers are not created!");
    vk::DeviceSize offsets[] = {0};
    auto buffer = vertexBuffer->GetBuffer();
    commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
    commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
}

void CVulkanVBO::addMeshData(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices)
{
    vVertices.insert(vVertices.end(), vertices.begin(), vertices.end());
    vIndices.insert(vIndices.end(), indices.begin(), indices.end());
}

void CVulkanVBO::createVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vVertices[0]) * vVertices.size();
    uint32_t vertexSize = sizeof(vVertices[0]);

    VulkanBuffer stagingBuffer;
    stagingBuffer.Create(vertexSize, vVertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.MapMem();
    stagingBuffer.Write((void *)vVertices.data());

    vertexBuffer = std::make_unique<VulkanBuffer>();
    vertexBuffer->Create(vertexSize, vVertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    UDevice->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
    // m_vVertices.clear();
}

void CVulkanVBO::createIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vIndices[0]) * vIndices.size();
    uint32_t indexSize = sizeof(vIndices[0]);

    VulkanBuffer stagingBuffer;
    stagingBuffer.Create(indexSize, vIndices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.MapMem();
    stagingBuffer.Write((void *)vIndices.data());

    indexBuffer = std::make_unique<VulkanBuffer>();
    indexBuffer->Create(indexSize, vIndices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    UDevice->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
    // m_vIndices.clear();
}