#include "ResourceManager.h"
#include "ResourceFactory.h"
#include "filesystem/FilesystemHelper.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    void ResourceManager::Load(std::string srResourcesPath)
    {
        auto input = FilesystemHelper::ReadFile(srResourcesPath);
        auto res_json = nlohmann::json::parse(input).front();

        std::vector<Engine::FTextureCreateInfo> vTextures;
        std::vector<Engine::FMaterialCreateInfo> vMaterials;
        std::vector<Engine::FMeshCreateInfo> vMeshes;

        res_json.at("textures").get_to(vTextures);
        res_json.at("materials").get_to(vMaterials);
        res_json.at("meshes").get_to(vMeshes);

        for(auto texture : vTextures)
            m_mTextures.emplace(texture.srName, ResourceFactory::CreateTexture(shared_from_this(), texture));

        for(auto material : vMaterials)
            m_mMaterials.emplace(material.srName, ResourceFactory::CreateMaterial(shared_from_this(), material));

        for(auto mesh : vMeshes)
            m_mMeshes.emplace(mesh.srName, ResourceFactory::CreateMesh(shared_from_this(), mesh));
    }

    void ResourceManager::DestroyAll()
    {
        for(auto& [key, value] : m_mTextures)
            value->Destroy();
        for(auto& [key, value] : m_mMaterials)
            value->Destroy();
        for(auto& [key, value] : m_mMeshes)
            value->Destroy();
    }
}