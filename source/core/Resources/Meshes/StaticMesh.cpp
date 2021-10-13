#include "StaticMesh.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

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

    void StaticMesh::CalcTangent()
    {
        for(size_t i = 0; i < indices.size(); i += 3)
        {
            glm::vec3 vertex0 = vertices.at(indices.at(i)).pos;
            glm::vec3 vertex1 = vertices.at(indices.at(i + 1)).pos;
            glm::vec3 vertex2 = vertices.at(indices.at(i + 2)).pos;

            glm::vec3 normal = glm::cross((vertex1 - vertex0),(vertex2 - vertex0));

            glm::vec3 deltaPos;
            if(vertex0 == vertex1)
                deltaPos = vertex2 - vertex0;
            else
                deltaPos = vertex1 - vertex0;
            
            glm::vec2 uv0 = vertices.at(indices.at(i)).texcoord;
            glm::vec2 uv1 = vertices.at(indices.at(i + 1)).texcoord;
            glm::vec2 uv2 = vertices.at(indices.at(i + 2)).texcoord;

            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            glm::vec3 tan, bin;
            if(deltaUV1.s != 0)
                tan = deltaPos / deltaUV1.s;
            else
                tan = deltaPos / 1.0f;

            tan = glm::normalize(tan - glm::dot(normal,tan)*normal);
            bin = glm::normalize(glm::cross(tan, normal));

            for(size_t j = 0; j < 3; j++)
            {
                vertices.at(indices.at(i + j)).tangent = tan;
                vertices.at(indices.at(i + j)).binormal = bin;
            }
        }
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

        CalcTangent();
    }
}