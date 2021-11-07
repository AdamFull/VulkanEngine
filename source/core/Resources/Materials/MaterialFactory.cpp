#include "MaterialFactory.h"
#include "Resources/Textures/TextureFactory.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanAllocator.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Material;

std::map<EMaterialType, std::function<MaterialFactory::material_t()>> MaterialFactory::m_mFactory{
    {EMaterialType::eUI, []()
     {
         auto material = Core::FDefaultAllocator::Allocate<MaterialUI>();
         return material;
     }},
    {EMaterialType::eDiffuse, []()
     {
         auto material = Core::FDefaultAllocator::Allocate<MaterialDiffuse>();
         return material;
     }},
    {EMaterialType::eSkybox, []()
     {
         auto material = Core::FDefaultAllocator::Allocate<MaterialSkybox>();
         return material;
     }}};

std::shared_ptr<MaterialBase> MaterialFactory::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMaterialCreateInfo info)
{
    auto material = m_mFactory[info.eType]();

    if (!info.srAttachments.empty())
    {
        for (auto &texName : info.srAttachments)
        {
            std::shared_ptr<Texture::TextureBase> texture = resourceMgr->Get<Texture::TextureBase>(texName);
            material->AddTexture(texture->GetAttachment(), texture);
        }
    }
    else
    {
        for (auto &texInfo : info.vTextures)
        {
            std::shared_ptr<Texture::TextureBase> texture = Texture::TextureFactory::Create(resourceMgr, texInfo);
            resourceMgr->AddExisting<Texture::TextureBase>(texInfo.srName, texture);
            material->AddTexture(texInfo.eAttachment, texture);
        }
    }

    material->AddTexture(ETextureAttachmentType::eEmpty, resourceMgr->Get<Texture::TextureBase>("no_texture"));

    return material;
}