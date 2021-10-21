#include "MeshLoader.h"
#include <filesystem>
namespace fs = std::filesystem;

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

namespace Engine
{
    bool MeshLoader::Load(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
    {
        fs::path filepath{srPath};

        if(filepath.extension() == ".obj")
            return LoadObj(srPath, vertices, indices, bLoadMaterial);
        else if(filepath.extension() == ".nif")
            return LoadNif(srPath, vertices, indices, bLoadMaterial);
        else if(filepath.extension() == ".fbx")
            return LoadFbx(srPath, vertices, indices, bLoadMaterial);
        else if(filepath.extension() == ".stl")
            return LoadStl(srPath, vertices, indices, bLoadMaterial);
        else if(filepath.extension() == ".gltf")
            return LoadGltf(srPath, vertices, indices, bLoadMaterial);

        return false;
    }

    bool MeshLoader::LoadObj(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
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

        #pragma omp parallel for
        for (const auto &shape : shapes)
        {
            uint32_t i = 0;
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

                vertex.tangent = {};
                vertex.binormal = {};

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);

                i++;
                if (!(i % 3)) 
                {
                    size_t indicesSize = indices.size();
                    uint32_t i0 = indices[indicesSize - 3];
                    uint32_t i1 = indices[indicesSize - 2];
                    uint32_t i2 = indices[indicesSize - 1];
                    Vertex &v0 = vertices[i0];
                    Vertex &v1 = vertices[i1];
                    Vertex &v2 = vertices[i2];

                    glm::vec2 duv1 = v1.texcoord - v0.texcoord;
                    glm::vec2 duv2 = v2.texcoord - v0.texcoord;
                    float k = 1 / (duv1.x * duv2.y - duv2.x * duv1.y);
                    glm::mat2x2 UV(duv2.y, -duv1.y, -duv2.x, duv1.x);
                    glm::mat2x3 E(v1.pos - v0.pos, v2.pos - v0.pos);
                    glm::mat2x3 TB = k * E * UV;

                    v0.tangent += TB[0];
                    v0.binormal += TB[1];
                    v1.tangent += TB[0];
                    v1.binormal += TB[1];
                    v2.tangent += TB[0];
                    v2.binormal += TB[1];
                }
            }
        }
        CalculateTangents(vertices, indices);
        return true;
    }

    bool MeshLoader::LoadNif(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
    {
        return true;
    }

    bool MeshLoader::LoadFbx(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
    {
        return true;
    }

    bool MeshLoader::LoadStl(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
    {
        return true;
    }

    bool MeshLoader::LoadGltf(std::string srPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool bLoadMaterial)
    {
        return true;
    }

    void MeshLoader::CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        #pragma omp parallel for
        for (int i = 0; i < vertices.size(); i++) 
        {
            vertices[i].tangent = glm::normalize(vertices[i].tangent);
            vertices[i].binormal = glm::normalize(vertices[i].binormal);
            glm::vec3 &T = vertices[i].tangent;
            glm::vec3 &B = vertices[i].binormal;
            glm::vec3 &N = vertices[i].normal;
            T = glm::normalize(T - glm::dot(T, N) * N);
            B = glm::cross(N, T);
        }
    }

    glm::vec4 MeshLoader::GenerateNormal(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c)
    {
    glm::vec4 p0(a);
    glm::vec4 p1(b);
    glm::vec4 p2(c);
    glm::vec4 u = p1 - p0;
    glm::vec4 v = p2 - p0;
    glm::vec4 p = gmath::Vector::cross(u, v);
    p = glm::normalize(p);
    return (gmath::Point)p;
    }
}