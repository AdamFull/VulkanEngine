#include "MaterialFactory.h"
#include "Core/Image/TextureFactory.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/MaterialUI.h"
#include "Resources/Materials/MaterialDiffuse.h"
#include "Resources/Materials/MaterialSkybox.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Material;

std::map<EMaterialType, std::function<MaterialFactory::material_t()>> MaterialFactory::m_mFactory{
    {EMaterialType::eUI, []()
     {
         auto material = std::make_unique<MaterialUI>();
         return material;
     }},
    {EMaterialType::eDiffuse, []()
     {
         auto material = std::make_unique<MaterialDiffuse>();
         return material;
     }},
    {EMaterialType::eSkybox, []()
     {
         auto material = std::make_unique<MaterialSkybox>();
         return material;
     }}};

std::shared_ptr<MaterialBase> MaterialFactory::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMaterialCreateInfo info)
{
    auto material = m_mFactory[info.eType]();

    for (auto &texInfo : info.vTextures)
    {
        std::shared_ptr<Core::Image> texture = Core::TextureFactory::Create(resourceMgr, texInfo);
        resourceMgr->AddExisting<Core::Image>(texInfo.srName, texture);
        material->AddTexture(texInfo.eAttachment, texture);
    }

    return material;
}