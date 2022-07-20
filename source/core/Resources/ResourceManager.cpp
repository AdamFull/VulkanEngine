#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/image/Image2D.h"

using namespace engine::core;
using namespace engine::resources;
using namespace engine::resources::material;

CResourceManager::~CResourceManager()
{
    cleanup();
}

void CResourceManager::create()
{
    //Creating empty texture
    auto pEmptyTexture = utl::make_ref<CImage>("empty.ktx2");
    addExisting("no_texture", pEmptyTexture);
}

void CResourceManager::cleanup()
{
    m_mTextures.clear();
    m_mMaterials.clear();
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
}