#include "VulkanMesh.h"
#include "VulkanMainStructures.h"
#include "VulkanDevice.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

namespace Engine::Types::Mesh
{
    void Load(Main::FVulkanEngine& engine, Static& mesh, std::string srMeshPath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, srMeshPath.c_str())) 
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<FVertex, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes)
        {
            for (const auto& index : shape.mesh.indices) 
            {
                FVertex vertex{};

                vertex.pos += 
                glm::vec3{
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = 
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0) 
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(vertex);
                }

                mesh.indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}