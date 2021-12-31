#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "serializer/JsonSerializer.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Mesh;
namespace Engine
{
    namespace Resources
    {
        void ResourceManager::Create()
        {
            std::shared_ptr<Image> pEmptyTexture = std::make_shared<Image>();
            pEmptyTexture->CreateEmptyTexture(512, 512, 1, 2, 0x8C43);
            AddExisting("no_texture", pEmptyTexture);
        }

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

            for (auto texture : vTextures)
                Add<Image>(texture);

            for (auto material : vMaterials)
                Add<MaterialBase>(material);

            for (auto mesh : vMeshes)
                Add<MeshFragment>(mesh);
        }
    }
}