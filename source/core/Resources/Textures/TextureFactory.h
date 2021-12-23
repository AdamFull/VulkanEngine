#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Texture
        {
            class TextureBase;
            struct TextureFactory
            {
                using texture_t = std::unique_ptr<TextureBase>;

            public:
                static texture_t Create(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info);

            private:
                static std::map<ETextureType, std::function<texture_t(FTextureCreateInfo)>> m_mFactory;
            };
        }
    }
}