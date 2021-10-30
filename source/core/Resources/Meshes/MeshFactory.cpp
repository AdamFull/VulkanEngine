#include "MeshFactory.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/MaterialFactory.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Resources/Meshes/VulkanMesh.h"
#include "MeshLoader.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Mesh;

std::map<EMeshType, std::function<std::shared_ptr<MeshBase>(FMeshCreateInfo)>>
    MeshFactory::m_mFactory{
        {EMeshType::eStatic, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshBase>();
             return mesh;
         }},
        {EMeshType::eSkeletal, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshBase>();
             return mesh;
         }},
        {EMeshType::eGLTF, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshBase>();
             return mesh;
         }}};

std::shared_ptr<MeshBase> MeshFactory::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMeshCreateInfo info)
{
    auto mesh = m_mFactory[info.eType](info);
    Loaders::MeshLoader::Load(info.srSrc, resourceMgr, mesh, info.bUseIncludedMaterial);

    if (!info.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.vMaterials)
        {
            std::shared_ptr<Material::MaterialBase> material = Material::MaterialFactory::Create(resourceMgr, matInfo);
            resourceMgr->AddExisting<Material::MaterialBase>(matInfo.srName, material);
            mesh->SetMaterial(material);
        }
    }
    return mesh;
}
