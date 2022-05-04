#include "MeshLoader.h"

using namespace Engine::Resources::Loaders;

bool CMeshLoader::load(std::string srPath, std::shared_ptr<Mesh::CMeshFragment> pMesh, bool bLoadMaterial)
{
    fs::path filepath{srPath};

    /*if(filepath.extension() == ".obj")
        return Loaders::Model::ObjLoader::Load();
    else if(filepath.extension() == ".nif")
        return LoadNif(srPath, pResourceManager, pMesh, bLoadMaterial);
    else if(filepath.extension() == ".fbx")
        return LoadFbx(srPath, pResourceManager, pMesh, bLoadMaterial);
    else if(filepath.extension() == ".stl")
        return LoadStl(srPath, pResourceManager, pMesh, bLoadMaterial);
    else if(filepath.extension() == ".gltf")
        return LoadGltf(srPath, pResourceManager, pMesh, bLoadMaterial);*/

    return false;
}

void CMeshLoader::calculateTangents(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices)
{
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++)
    {
        /*vertices[i].tangent = glm::normalize(vertices[i].tangent);
        vertices[i].binormal = glm::normalize(vertices[i].binormal);
        glm::vec3 &T = vertices[i].tangent;
        glm::vec3 &B = vertices[i].binormal;
        glm::vec3 &N = vertices[i].normal;
        T = glm::normalize(T - glm::dot(T, N) * N);
        B = glm::cross(N, T);*/
    }
}

glm::vec4 CMeshLoader::generateNormals(std::vector<Core::FVertex> &vertices, std::vector<uint32_t> indices)
{
    return glm::vec4{};
}
