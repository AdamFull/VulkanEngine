#include "VulkanMesh.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

namespace Engine
{
    void MeshBase::Create(std::string srResourcePath)
    {
        
    }

    void MeshBase::ReCreate()
    {
        ResourceBase::ReCreate();
    }

    void MeshBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
    }

    void MeshBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);

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
    }

    void MeshBase::Destroy()
    {
        ResourceBase::Destroy();

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

    /***************************************************StaticMesh*********************************************************************/
    void StaticMesh::Create(std::string srResourcePath)
    {
        MeshBase::Create(srResourcePath);

        Load(srResourcePath);

        if(!vertices.empty())
        CreateVertexBuffer();
        if(!indices.empty())
        CreateIndexBuffer();
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

    //Static mesh
    void StaticMesh::Load(std::string srPath)
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
                Vertex vertex{};

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
    }
}