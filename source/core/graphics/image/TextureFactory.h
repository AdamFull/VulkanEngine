#pragma once
#include "resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
    }
    namespace Core
    {
        class Image;
        struct TextureFactory
        {
            using texture_t = std::unique_ptr<Image>;

        public:
            static texture_t Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, Resources::FTextureCreateInfo info);

        private:
            static std::map<Resources::ETextureType, std::function<texture_t(Resources::FTextureCreateInfo)>> m_mFactory;
        };
    }
}