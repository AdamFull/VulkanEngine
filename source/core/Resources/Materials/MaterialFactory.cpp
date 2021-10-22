#include "MaterialFactory.h"
#include "Resources/Textures/TextureFactory.h"
#include "Resources/ResourceManager.h"

#include "Resources/Materials/MaterialUI.h"
#include "Resources/Materials/MaterialDiffuse.h"
#include "Resources/Materials/MaterialSkybox.h"
#include "Resources/Materials/MaterialPBR.h"

namespace Engine
{
    std::map<EMaterialType, std::function<MaterialFactory::material_t()>> MaterialFactory::m_mFactory
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
        },
        {
            EMaterialType::ePBR, []()
            {
                auto material = std::make_unique<MaterialPBR>();
                material->Create();
                return material;
            }
        }
    };

    std::shared_ptr<MaterialBase> MaterialFactory::Create(std::shared_ptr<ResourceManager> resourceMgr, FMaterialCreateInfo info)
    {
        auto material = m_mFactory[info.eType]();

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
                std::shared_ptr<TextureBase> texture = TextureFactory::Create(resourceMgr, texInfo);
                resourceMgr->AddExisting<TextureBase>(texInfo.srName, texture);
                material->AddTexture(texInfo.eAttachment, texture);
            }
        }

        return material;
    }
}