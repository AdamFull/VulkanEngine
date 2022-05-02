#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Mesh;
namespace Engine
{
    namespace Resources
    {
        void CResourceManager::create()
        {
            std::shared_ptr<CImage> pEmptyTexture = std::make_shared<CImage>();
            pEmptyTexture->createEmptyTexture(512, 512, 1, 2, 0x8C43);
            addExisting("no_texture", pEmptyTexture);
        }

        void CResourceManager::load(std::string srResourcesPath)
        {
            auto input = FilesystemHelper::readFile(srResourcesPath);
            auto res_json = nlohmann::json::parse(input).front();

            std::vector<FTextureCreateInfo> vTextures;
            std::vector<FMaterialCreateInfo> vMaterials;
            std::vector<FMeshCreateInfo> vMeshes;

            res_json.at("textures").get_to(vTextures);
            res_json.at("materials").get_to(vMaterials);
            res_json.at("meshes").get_to(vMeshes);

            for (auto texture : vTextures)
                Add<CImage>(texture);

            for (auto material : vMaterials)
                Add<CMaterialBase>(material);

            for (auto mesh : vMeshes)
                Add<CMeshFragment>(mesh);
        }
    }
}