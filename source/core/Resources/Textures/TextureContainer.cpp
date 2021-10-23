#include "TextureContainer.h"
#include "VulkanTexture.h"

namespace Engine
{
    void TextureContainer::Push(std::shared_ptr<TextureBase> texture)
    {
        m_vTextures.emplace_back(texture);
    }

    void TextureContainer::Pop()
    {
        auto texture = m_vTextures.back();
        texture->Destroy();
        m_vTextures.pop_back();
    }

    std::shared_ptr<TextureBase> TextureContainer::Get(TextureContainer::texture_container_t::size_type index)
    {
        return m_vTextures.at(index);
    }

    void TextureContainer::Destroy()
    {
        for(auto& texture : m_vTextures)
            texture->Destroy();
        m_vTextures.clear();
    }

}