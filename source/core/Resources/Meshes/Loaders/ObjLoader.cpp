#include "ObjLoader.h"
#include "Objects/RenderObject.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

using namespace Engine::Resources::Loaders;

void ObjLoader::Load(std::string srPath, std::shared_ptr<Objects::RenderObject> pRoot)
{
    /*tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // UVBO->GetLastIndex()

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, srPath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::cout << warn << std::endl;
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

#pragma omp parallel for
    for (size_t shape_index = 0; shape_index < shapes.size(); shape_index++) // const auto &shape : shapes
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        auto &shape = shapes[shape_index];

        Primitive primitive;
        primitive.firstIndex = UVBO->GetLastIndex();
        primitive.firstVertex = UVBO->GetLastVertex();
        uint32_t i = 0;
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.pos += glm::vec3{
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]};

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
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
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

        CalculateTangents(vertices, indices);
        primitive.indexCount = indices.size();
        primitive.vertexCount = indices.size();

        if (bLoadMaterial)
        {
            // TODO: Create adapters
            auto &material = materials[i];
            FTextureCreateInfo textureDiffuse{};
            textureDiffuse.srName = shape.name + "_diffuse";
            textureDiffuse.eType = ETextureType::e2D;
            textureDiffuse.eAttachment = ETextureAttachmentType::eColor;
            textureDiffuse.srSrc = material.diffuse_texname;

            FTextureCreateInfo textureNormal{};
            textureNormal.srName = shape.name + "_normal";
            textureNormal.eType = ETextureType::e2D;
            textureNormal.eAttachment = ETextureAttachmentType::eNormal;
            textureNormal.srSrc = material.normal_texname;

            FTextureCreateInfo textureSpecular{};
            textureSpecular.srName = shape.name + "_specular";
            textureSpecular.eType = ETextureType::e2D;
            textureSpecular.eAttachment = ETextureAttachmentType::eSpecular;
            textureSpecular.srSrc = material.specular_texname;

            FMaterialCreateInfo materialInfo{};
            materialInfo.srName = shape.name + "_material";
            materialInfo.eType = EMaterialType::eDiffuse;
            materialInfo.vTextures = {textureDiffuse, textureNormal, textureSpecular};

            primitive.material = MaterialFactory::Create(pResourceManager, materialInfo);
        }

        UVBO->AddMeshData(std::move(vertices), std::move(indices));
        pMesh->AddPrimitive(shape.name, std::move(primitive));
    }

    return true;*/
}
