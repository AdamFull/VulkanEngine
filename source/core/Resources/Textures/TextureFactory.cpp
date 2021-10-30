#include "TextureFactory.h"
#include "Resources/ResourceManager.h"

#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCubemap.h"
#include "Texture2DArray.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Texture;

std::map<ETextureType, std::function<TextureFactory::texture_t(FTextureCreateInfo)>> TextureFactory::m_mFactory{
    // Check is creating from source or from data
    {
        ETextureType::e2D, [](FTextureCreateInfo info)
        {
            auto texture = std::make_unique<Texture2D>();
            texture->SetAttachment(info.eAttachment);
            texture->LoadFromFile(info.srSrc);
            return texture;
        }},
    {ETextureType::e3D, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<Texture3D>();
         texture->SetAttachment(info.eAttachment);
         texture->LoadNoise(info.noise.pattern, info.noise.width, info.noise.height, info.noise.depth);
         return texture;
     }},
    {ETextureType::eCubemap, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<TextureCubemap>();
         texture->SetAttachment(info.eAttachment);
         texture->LoadFromFile(info.srSrc);
         return texture;
     }},
    {ETextureType::eArray, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<Texture2DArray>();
         texture->SetAttachment(info.eAttachment);
         texture->LoadFromFile(info.srSrc);
         return texture;
     }}};

TextureFactory::texture_t TextureFactory::Create(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info)
{
    return m_mFactory[info.eType](info);
}