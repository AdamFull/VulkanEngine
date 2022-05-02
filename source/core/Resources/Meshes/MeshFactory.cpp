#include "MeshFactory.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/MaterialFactory.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Resources/Meshes/MeshFragment.h"
#include "MeshLoader.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Mesh;

std::map<EMeshType, std::function<std::shared_ptr<CMeshFragment>(FMeshCreateInfo)>>
    CMeshFactory::m_mFactory{
        {EMeshType::eStatic, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<CMeshFragment>();
             return mesh;
         }},
        {EMeshType::eSkeletal, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<CMeshFragment>();
             return mesh;
         }},
        {EMeshType::eGLTF, [](FMeshCreateInfo info)
         {
             auto mesh = std::make_shared<CMeshFragment>();
             return mesh;
         }}};

std::shared_ptr<CMeshFragment> CMeshFactory::create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMeshCreateInfo info)
{
    auto mesh = m_mFactory[info.eType](info);
    Loaders::CMeshLoader::load(info.srSrc, resourceMgr, mesh, info.bUseIncludedMaterial);

    if (!info.bUseIncludedMaterial)
    {
        for (auto &matInfo : info.vMaterials)
        {
            std::shared_ptr<Material::CMaterialBase> material = Material::CMaterialFactory::create(resourceMgr, matInfo);
            resourceMgr->addExisting<Material::CMaterialBase>(matInfo.srName, material);
            mesh->setMaterial(material);
        }
    }
    return mesh;
}
