#include "MeshFactory.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/MaterialFactory.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Resources/Meshes/MeshFragment.h"
#include "MeshLoader.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Mesh;

std::map<EMeshType, std::function<std::shared_ptr<MeshFragment>(FMeshCreateInfo)>>
    MeshFactory::m_mFactory{
        {EMeshType::eStatic, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshFragment>();
             return mesh;
         }},
        {EMeshType::eSkeletal, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshFragment>();
             return mesh;
         }},
        {EMeshType::eGLTF, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<MeshFragment>();
             return mesh;
         }}};

std::shared_ptr<MeshFragment> MeshFactory::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMeshCreateInfo info)
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
