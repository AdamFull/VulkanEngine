#include "ResourceFactory.h"
#include "Materials/MaterialUI.h"
#include "Materials/MaterialDiffuse.h"

namespace Engine
{
    std::map<ETextureType, std::function<ResourceFactory::texture_t()>> ResourceFactory::m_mTextureFactory;
    std::map<EMaterialType, std::function<ResourceFactory::material_t()>> ResourceFactory::m_mMaterialFactory;
    std::map<EMeshType, std::function<ResourceFactory::mesh_t()>> ResourceFactory::m_mMeshFactory;

    ResourceFactory::texture_t ResourceFactory::CreateResource(FTextureCreateInfo info)
    {
        auto texture = m_mTextureFactory[info.eType]();
        return texture;
    }

    ResourceFactory::material_t ResourceFactory::CreateResource(FMaterialCreateInfo info)
    {
        auto material = m_mMaterialFactory[info.eType]();

        for(auto& texInfo : info.vTextures)
            material->AddTexture(texInfo.eAttachment, CreateResource(texInfo));

        return material;
    }

    ResourceFactory::mesh_t ResourceFactory::CreateResource(FMeshCreateInfo info)
    {
        auto mesh = m_mMeshFactory[info.eType]();

        for(auto& matInfo : info.vMaterials)
        {
            mesh->AddMaterial(CreateResource(matInfo));
        }

        return mesh;
    }
}