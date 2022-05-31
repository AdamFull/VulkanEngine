#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/image/Image2D.h"

using namespace Engine::Core;
using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Resources::Mesh;

template <>
scope_ptr<CResourceManager> utl::singleton<CResourceManager>::_instance{nullptr};

void CResourceManager::create()
{
    //Creating empty texture
    auto pEmptyTexture = make_ref<CImage>();
    pEmptyTexture->create("empty.png");
    addExisting("no_texture", pEmptyTexture);
}

void CResourceManager::load(std::string srResourcesPath)
{
    //Not sure that this method still works
    //TODO: check is this methos still uses
    auto input = FilesystemHelper::readFile(srResourcesPath);
    auto res_json = nlohmann::json::parse(input).front();

    std::vector<FTextureCreateInfo> vTextures;
    std::vector<FMaterialCreateInfo> vMaterials;
    std::vector<FMeshCreateInfo> vMeshes;

    res_json.at("textures").get_to(vTextures);
    res_json.at("materials").get_to(vMaterials);
    res_json.at("meshes").get_to(vMeshes);

    for (auto texture : vTextures)
        add<CImage>(texture);

    for (auto material : vMaterials)
        add<CMaterialBase>(material);

    for (auto mesh : vMeshes)
        add<CMeshFragment>(mesh);
}