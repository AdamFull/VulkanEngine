#include "VulkanMesh.h"
#include "VulkanDevice.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

namespace Engine
{
    void VulkanStaticMesh::LoadStaticMesh(std::unique_ptr<Device>& device, std::string srPath, FTransform sTransform)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, srPath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{sTransform.pos * sTransform.scale};

                vertex.pos += glm::vec3
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = 
                {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };

                if (index.normal_index >= 0) 
                {
                    vertex.normal = 
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) 
                {
                    vertex.texcoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        if(!vertices.empty())
        CreateVertexBuffer(device);
        if(!indices.empty())
        CreateIndexBuffer(device);
    }

    void VulkanStaticMesh::Bind(vk::CommandBuffer& commandBuffer)
    {
        vk::Buffer vertexBuffers[] = {vertexBuffer};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
    }

    void VulkanStaticMesh::Draw(vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }

    void VulkanStaticMesh::CreateVertexBuffer(std::unique_ptr<Device>& device)
    {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        device->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        device->MoveToMemory(vertices.data(), stagingBufferMemory, bufferSize);
        device->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
        device->CopyOnDeviceBuffer(stagingBuffer, vertexBuffer, bufferSize);

        device->Destroy(stagingBuffer);
        device->Destroy(stagingBufferMemory);
    }

    void VulkanStaticMesh::CreateIndexBuffer(std::unique_ptr<Device>& device)
    {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        device->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        device->MoveToMemory(indices.data(), stagingBufferMemory, bufferSize);
        device->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indiciesBufferMemory);
        device->CopyOnDeviceBuffer(stagingBuffer, indexBuffer, bufferSize);

        device->Destroy(stagingBuffer);
        device->Destroy(stagingBufferMemory);
    }
}