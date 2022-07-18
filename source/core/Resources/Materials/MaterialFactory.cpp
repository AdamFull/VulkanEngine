#include "MaterialFactory.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::resources;
using namespace engine::resources::material;

utl::ref_ptr<CMaterialBase> CMaterialFactory::create(FMaterialCreateInfo info)
{
    auto material = CMaterialLoader::inst()->create(info.srName);

    for (auto &texInfo : info.vTextures)
    {
        auto texture = utl::make_ref<CImage>(texInfo.srSrc);
        CResourceManager::inst()->addExisting(texInfo.srName, texture);
        material->addTexture(texInfo.attachment, texture);
    }

    return material;
}