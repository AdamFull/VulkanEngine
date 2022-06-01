#include "MaterialFactory.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace engine::core;
using namespace engine::resources;
using namespace engine::resources::material;

ref_ptr<CMaterialBase> CMaterialFactory::create(FMaterialCreateInfo info)
{
    auto material = CMaterialLoader::inst()->create(info.srName);

    for (auto &texInfo : info.vTextures)
    {
        auto texture = make_ref<CImage>();
        texture->create(texInfo.srSrc);
        CResourceManager::inst()->addExisting(texInfo.srName, texture);
        material->addTexture(texInfo.attachment, texture);
    }

    return material;
}