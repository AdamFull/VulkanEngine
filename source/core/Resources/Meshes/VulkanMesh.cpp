#include "VulkanMesh.h"
#include "VulkanDevice.h"
#include "VulkanHighLevel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

namespace Engine
{
    std::array<vk::VertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[3].offset = offsetof(Vertex, texcoord);

        return attributeDescriptions;
    }

    //Vertex
    vk::VertexInputBindingDescription Vertex::getBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    void MeshBase::Create(std::string srResourcePath)
    {
        
    }

    void MeshBase::ReCreate()
    {
        ResourceBase::ReCreate();
    }

    void MeshBase::Update(uint32_t imageIndex)
    {
        ResourceBase::Update(imageIndex);
    }

    void MeshBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);

        vk::Buffer vertexBuffers[] = {vertexBuffer};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
        //Draw
        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }

    void MeshBase::Destroy()
    {
        ResourceBase::Destroy();

        UDevice->Destroy(vertexBuffer);
        UDevice->Destroy(vertexBufferMemory);
        UDevice->Destroy(indexBuffer);
        UDevice->Destroy(indiciesBufferMemory);
    }

    void MeshBase::CreateVertexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        UDevice->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        UDevice->MoveToMemory(vertices.data(), stagingBufferMemory, bufferSize);
        UDevice->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
        UDevice->CopyOnDeviceBuffer(stagingBuffer, vertexBuffer, bufferSize);

        UDevice->Destroy(stagingBuffer);
        UDevice->Destroy(stagingBufferMemory);
    }

    void MeshBase::CreateIndexBuffer()
    {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        UDevice->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        UDevice->MoveToMemory(indices.data(), stagingBufferMemory, bufferSize);
        UDevice->CreateOnDeviceBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indiciesBufferMemory);
        UDevice->CopyOnDeviceBuffer(stagingBuffer, indexBuffer, bufferSize);

        UDevice->Destroy(stagingBuffer);
        UDevice->Destroy(stagingBufferMemory);
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

    void StaticMesh::Update(uint32_t imageIndex)
    {
        MeshBase::Update(imageIndex);
    }

    void StaticMesh::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        MeshBase::Bind(commandBuffer, imageIndex);
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