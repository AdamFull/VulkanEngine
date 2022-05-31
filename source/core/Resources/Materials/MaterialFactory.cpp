#include "MaterialFactory.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Material;

ref_ptr<CMaterialBase> CMaterialFactory::create(FMaterialCreateInfo info)
{
    auto material = CMaterialLoader::inst()->create(info.srName);

    for (auto &texInfo : info.vTextures)
    {
        ref_ptr<Core::CImage> texture = make_ref<Core::CImage>();
        texture->create(texInfo.srSrc);
        CResourceManager::inst()->addExisting(texInfo.srName, texture);
        material->addTexture(texInfo.attachment, texture);
    }

    return material;
}