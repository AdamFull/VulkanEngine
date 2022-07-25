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