#pragma once 
#include "Resources/ResourceBase.h"

namespace Engine
{
    class TextureBase;
    class TextureContainer : public ResourceBase
    {
    public:
        using texture_container_t = std::vector<std::shared_ptr<TextureBase>>;

        void Push(std::shared_ptr<TextureBase> texture);
        void Pop();
        std::shared_ptr<TextureBase> Get(texture_container_t::size_type index);
        void Destroy() override;
    protected:
        texture_container_t m_vTextures;
    };
}