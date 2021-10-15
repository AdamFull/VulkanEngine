#include "ResourceFactory.h"
#include "ResourceManager.h"

#include "Resources/Textures/Texture2D.h"
#include "Resources/Textures/TextureCubemap.h"

#include "Resources/Materials/MaterialUI.h"
#include "Resources/Materials/MaterialDiffuse.h"
#include "Resources/Materials/MaterialSkybox.h"

#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/GLTFMesh.h"

namespace Engine
{
    std::map<ETextureType, std::function<ResourceFactory::texture_t(FTextureCreateInfo)>> ResourceFactory::m_mTextureFactory
    {
        //Check is creating from source or from data
        {
            ETextureType::e2D, [](FTextureCreateInfo info)
            {
                auto texture = std::make_unique<Texture2D>();
                texture->SetAttachment(info.eAttachment);
                texture->Load(info.srSrc);
                return texture;
            }
        },
        {
            ETextureType::e3D, [](FTextureCreateInfo info)
            {
                auto texture = std::make_unique<Texture2D>();
                texture->SetAttachment(info.eAttachment);
                texture->Load(info.srSrc);
                return texture;
            }
        },
        {
            ETextureType::eCubemap, [](FTextureCreateInfo info)
            {
                auto texture = std::make_unique<TextureCubemap>();
                texture->SetAttachment(info.eAttachment);
                texture->Load(info.srSrc);
                return texture;
            }
        }
    };

    std::map<EMaterialType, std::function<ResourceFactory::material_t()>> ResourceFactory::m_mMaterialFactory
    {
        {
            EMaterialType::eUI, []()
            {
                auto material = std::make_unique<MaterialUI>();
                material->Create();
                return material;
            }
        },
        {
            EMaterialType::eDiffuse, []()
            {
                auto material = std::make_unique<MaterialDiffuse>();
                material->Create();
                return material;
            }
        },
        {
            EMaterialType::eSkybox, []()
            {
                auto material = std::make_unique<MaterialSkybox>();
                material->Create();
                return material;
            }
        }
    };

    std::map<EMeshType, std::function<ResourceFactory::mesh_t(FMeshCreateInfo)>> ResourceFactory::m_mMeshFactory
    {
        {
            EMeshType::eStatic, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_unique<StaticMesh>();
                mesh->Load(info.srSrc);
                mesh->Create();
                return mesh;
            }
        },
        {
            EMeshType::eSkeletal, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_unique<StaticMesh>();
                mesh->Load(info.srSrc);
                mesh->Create();
                return mesh;
            }
        },
        {
            EMeshType::eGLTF, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_unique<StaticMesh>();
                mesh->Load(info.srSrc);
                mesh->Create();
                return mesh;
            }
        }
    };

    ResourceFactory::texture_t ResourceFactory::CreateTexture(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info)
    {
        return m_mTextureFactory[info.eType](info);
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
        auto mesh = m_mMeshFactory[info.eType](info);

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