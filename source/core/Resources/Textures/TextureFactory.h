#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    class TextureBase;
    class ResourceManager;

    struct TextureFactory
    {
        using texture_t = std::unique_ptr<TextureBase>;
    public:
        static texture_t Create(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info);
    private:
        static std::map<ETextureType, std::function<texture_t(FTextureCreateInfo)>> m_mFactory;
    };
}