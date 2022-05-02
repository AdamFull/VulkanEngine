#include "MaterialFactory.h"
#include "resources/ResourceManager.h"
#include "resources/materials/MaterialUI.h"
#include "resources/materials/MaterialDiffuse.h"
#include "resources/materials/MaterialSkybox.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Material;

std::map<EMaterialType, std::function<CMaterialFactory::material_t()>> CMaterialFactory::m_mFactory{
    {EMaterialType::eUI, []()
     {
         auto material = std::make_unique<CMaterialUI>();
         return material;
     }},
    {EMaterialType::eDiffuse, []()
     {
         auto material = std::make_unique<CMaterialDiffuse>();
         return material;
     }},
    {EMaterialType::eSkybox, []()
     {
         auto material = std::make_unique<CMaterialSkybox>();
         return material;
     }}};

std::shared_ptr<CMaterialBase> CMaterialFactory::create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMaterialCreateInfo info)
{
    auto material = m_mFactory[info.eType]();

    for (auto &texInfo : info.vTextures)
    {
        std::shared_ptr<Core::CImage> texture = std::make_shared<Core::CImage>();
        texture->loadFromFile(texInfo.srSrc);
        resourceMgr->addExisting<Core::CImage>(texInfo.srName, texture);
        material->addTexture(texInfo.attachment, texture);
    }

    return material;
}