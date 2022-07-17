#include "graphics/VulkanHighLevel.h"

using namespace engine::core;

void CVertexBufferObject::create()
{
    createVertexBuffer();
    createIndexBuffer();
    bBuffersCreated = true;
}

void CVertexBufferObject::cleanup()
{
    if(!bIsClean)
    {
        vertexBuffer->cleanup();
        indexBuffer->cleanup();
        bIsClean = true;
    }
}

void CVertexBufferObject::bind(vk::CommandBuffer commandBuffer)
{
    assert(bBuffersCreated && "Vertex and index buffers are not created!");
    vk::DeviceSize offsets[] = {0};
    auto buffer = vertexBuffer->getBuffer();
    commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
    commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
}

void CVertexBufferObject::addMeshData(std::vector<FVertex> &&vertices, std::vector<uint32_t> &&indices)
{
    vVertices.insert(vVertices.end(), vertices.begin(), vertices.end());
    vIndices.insert(vIndices.end(), indices.begin(), indices.end());
}

void CVertexBufferObject::createVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vVertices[0]) * vVertices.size();
    auto vertexSize = sizeof(vVertices[0]);

    CVulkanBuffer stagingBuffer;
    stagingBuffer.create(vertexSize, vVertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.mapMem();
    stagingBuffer.write((void *)vVertices.data());

    vertexBuffer = utl::make_scope<CVulkanBuffer>();
    vertexBuffer->create(vertexSize, vVertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    CDevice::inst()->copyOnDeviceBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    stagingBuffer.cleanup();
}

void CVertexBufferObject::createIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vIndices[0]) * vIndices.size();
    auto indexSize = sizeof(vIndices[0]);

    CVulkanBuffer stagingBuffer;
    stagingBuffer.create(indexSize, vIndices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.mapMem();
    stagingBuffer.write((void *)vIndices.data());

    indexBuffer = utl::make_scope<CVulkanBuffer>();
    indexBuffer->create(indexSize, vIndices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    CDevice::inst()->copyOnDeviceBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    stagingBuffer.cleanup();
}