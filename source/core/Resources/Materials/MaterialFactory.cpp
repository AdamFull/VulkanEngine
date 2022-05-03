#include "MaterialFactory.h"
#include "resources/ResourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Material;

std::shared_ptr<CMaterialBase> CMaterialFactory::create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMaterialCreateInfo info)
{
    auto material = CMaterialLoader::getInstance()->create(info.srName);

    for (auto &texInfo : info.vTextures)
    {
        std::shared_ptr<Core::CImage> texture = std::make_shared<Core::CImage>();
        texture->loadFromFile(texInfo.srSrc);
        resourceMgr->addExisting<Core::CImage>(texInfo.srName, texture);
        material->addTexture(texInfo.attachment, texture);
    }

    return material;
}