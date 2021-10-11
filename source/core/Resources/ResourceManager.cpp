#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    void ResourceManager::Load(std::string srResourcesPath)
    {
        auto input = FilesystemHelper::ReadFile(srResourcesPath);
        auto res_json = nlohmann::json::parse(input).front();

        std::vector<FTextureCreateInfo> vTextures;
        std::vector<FMaterialCreateInfo> vMaterials;
        std::vector<FMeshCreateInfo> vMeshes;

        res_json.at("textures").get_to(vTextures);
        res_json.at("materials").get_to(vMaterials);
        res_json.at("meshes").get_to(vMeshes);

        for(auto texture : vTextures)
            Add<TextureBase>(texture);

        for(auto material : vMaterials)
            Add<MaterialBase>(material);

        for(auto mesh : vMeshes)
            Add<MeshBase>(mesh);
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