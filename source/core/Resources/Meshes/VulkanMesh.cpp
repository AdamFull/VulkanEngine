#include "VulkanMesh.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void MeshBase::Create(std::string srResourcePath)
    {
        
    }

    void MeshBase::AddMaterial(std::shared_ptr<MaterialBase> material)
    {
        vMaterials.emplace_back(material);
    }

    void MeshBase::ReCreate()
    {
        ResourceBase::ReCreate();
        vMaterials.front()->ReCreate();
    }

    void MeshBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
        vMaterials.front()->Update(imageIndex, pUniformBuffer);
    }

    void MeshBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        vMaterials.front()->Bind(commandBuffer, imageIndex);

        vk::Buffer vertexBuffers[] = {vertexBuffer->GetBuffer()};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
        //Draw
        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }

    void MeshBase::Cleanup()
    {
        ResourceBase::Cleanup();
        vMaterials.front()->Cleanup();
    }

    void MeshBase::Destroy()
    {
        ResourceBase::Destroy();
        vMaterials.front()->Destroy();

        vertexBuffer->Destroy(UDevice);
        indexBuffer->Destroy(UDevice);
    }

    void MeshBase::CreateVertexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        uint32_t vertexSize = sizeof(vertices[0]);

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(UDevice, vertexSize, vertices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(UDevice);
        stagingBuffer.Write(UDevice, (void*)vertices.data());

        vertexBuffer = std::make_unique<VulkanBuffer>();
        vertexBuffer->Create(UDevice, vertexSize, vertices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
        UDevice->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

        stagingBuffer.Destroy(UDevice);
    }

    void MeshBase::CreateIndexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();
        uint32_t indexSize = sizeof(indices[0]);

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(UDevice, indexSize, indices.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.MapMem(UDevice);
        stagingBuffer.Write(UDevice, (void*)indices.data());

        indexBuffer = std::make_unique<VulkanBuffer>();
        indexBuffer->Create(UDevice, indexSize, indices.size(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
        UDevice->CopyOnDeviceBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

        stagingBuffer.Destroy(UDevice);
    }
}