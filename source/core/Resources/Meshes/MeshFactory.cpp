#include "MeshFactory.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/MaterialFactory.h"
#include "MeshLoader.h"

namespace Engine
{
    std::map<EMeshType, std::function<MeshFactory::mesh_t(FMeshCreateInfo)>> MeshFactory::m_mFactory
    {
        {
            EMeshType::eStatic, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_shared<MeshBase>();
                mesh->Create();
                return mesh;
            }
        },
        {
            EMeshType::eSkeletal, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_shared<MeshBase>();
                mesh->Create();
                return mesh;
            }
        },
        {
            EMeshType::eGLTF, [](FMeshCreateInfo info)
            {
                auto mesh = std::make_shared<MeshBase>();
                mesh->Create();
                return mesh;
            }
        }
    };

    MeshFactory::mesh_t MeshFactory::Create(std::shared_ptr<ResourceManager> resourceMgr, FMeshCreateInfo info)
    {
        auto mesh = m_mFactory[info.eType](info);
        MeshLoader::Load(info.srSrc, resourceMgr, mesh, info.bUseIncludedMaterial);

        if(!info.bUseIncludedMaterial)
        {
            for (auto &matInfo : info.vMaterials)
            {
                std::shared_ptr<MaterialBase> material = MaterialFactory::Create(resourceMgr, matInfo);
                resourceMgr->AddExisting<MaterialBase>(matInfo.srName, material);
                mesh->SetMaterial(material);
            }
        }
        return mesh;
    }
}