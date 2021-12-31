#include "TextureFactory.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"

#include "Image3D.h"

using namespace Engine::Core;
using namespace Engine::Resources;

std::map<ETextureType, std::function<TextureFactory::texture_t(FTextureCreateInfo)>> TextureFactory::m_mFactory{
    // Check is creating from source or from data
    {
        ETextureType::e2D, [](FTextureCreateInfo info)
        {
            auto texture = std::make_unique<Image2D>();
            texture->LoadFromFile(info.srSrc);
            return texture;
        }},
    {ETextureType::e3D, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<Image3D>();
         texture->LoadNoise(info.noise.pattern, info.noise.width, info.noise.height, info.noise.depth);
         return texture;
     }},
    {ETextureType::eCubemap, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<ImageCubemap>();
         texture->LoadFromFile(info.srSrc);
         return texture;
     }},
    {ETextureType::eArray, [](FTextureCreateInfo info)
     {
         auto texture = std::make_unique<Image2DArray>();
         texture->LoadFromFile(info.srSrc);
         return texture;
     }}};

TextureFactory::texture_t TextureFactory::Create(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info)
{
    return m_mFactory[info.eType](info);
}