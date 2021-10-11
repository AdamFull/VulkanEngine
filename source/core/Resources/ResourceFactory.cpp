#include "ResourceFactory.h"
#include "ResourceManager.h"

#include "Resources/Textures/Texture2D.h"
#include "Resources/Textures/TextureCubemap.h"

#include "Resources/Materials/MaterialUI.h"
#include "Resources/Materials/MaterialDiffuse.h"

#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/GLTFMesh.h"

namespace Engine
{
    std::map<ETextureType, std::function<ResourceFactory::texture_t()>> ResourceFactory::m_mTextureFactory;
    std::map<EMaterialType, std::function<ResourceFactory::material_t()>> ResourceFactory::m_mMaterialFactory;
    std::map<EMeshType, std::function<ResourceFactory::mesh_t()>> ResourceFactory::m_mMeshFactory;

    ResourceFactory::texture_t ResourceFactory::CreateTexture(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info)
    {
        auto texture = m_mTextureFactory[info.eType]();
        texture->SetAttachment(info.eAttachment);
        return texture;
    }

    ResourceFactory::material_t ResourceFactory::CreateMaterial(std::shared_ptr<ResourceManager> resourceMgr, FMaterialCreateInfo info)
    {
        auto material = m_mMaterialFactory[info.eType]();

        if(!info.srAttachments.empty())
        {
            for(auto& texName : info.srAttachments)
            {
                std::shared_ptr<TextureBase> texture = resourceMgr->Get<TextureBase>(texName);
                material->AddTexture(texture->GetAttachment(), texture);
            }
        }
        else
        {
            for(auto& texInfo : info.vTextures)
            {
                std::shared_ptr<TextureBase> texture = CreateTexture(resourceMgr, texInfo);
                resourceMgr->AddExisting<TextureBase>(texInfo.srName, texture);
                material->AddTexture(texInfo.eAttachment, texture);
            }
        }

        return material;
    }

    ResourceFactory::mesh_t ResourceFactory::CreateMesh(std::shared_ptr<ResourceManager> resourceMgr, FMeshCreateInfo info)
    {
        auto mesh = m_mMeshFactory[info.eType]();

        if(!info.srAttachments.empty())
        {
            for(auto& matName : info.srAttachments)
            {
                std::shared_ptr<MaterialBase> material = resourceMgr->Get<MaterialBase>(matName);
                mesh->AddMaterial(material);
            }
        }
        else
        {
            for(auto& matInfo : info.vMaterials)
            {
                std::shared_ptr<MaterialBase> material = CreateMaterial(resourceMgr, matInfo);
                resourceMgr->AddExisting<MaterialBase>(matInfo.srName, material);
                mesh->AddMaterial(material);
            }
        }

        return mesh;
    }
}