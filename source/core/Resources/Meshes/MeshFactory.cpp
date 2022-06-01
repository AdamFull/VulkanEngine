#include "MeshFactory.h"
#include "resources/ResourceManager.h"
#include "resources/meshes/MeshFragment.h"
#include "resources/materials/MaterialFactory.h"
#include "resources/materials/VulkanMaterial.h"
#include "MeshLoader.h"

using namespace engine::resources;
using namespace engine::resources::loaders;
using namespace engine::resources::mesh;

std::map<EMeshType, std::function<ref_ptr<CMeshFragment>(FMeshCreateInfo)>>
    CMeshFactory::m_mFactory{
        {EMeshType::eStatic, [](FMeshCreateInfo info)
         {
             auto mesh = make_ref<CMeshFragment>();
             return mesh;
         }},
        {EMeshType::eSkeletal, [](FMeshCreateInfo info)
         {
             auto mesh = make_ref<CMeshFragment>();
             return mesh;
         }},
        {EMeshType::eGLTF, [](FMeshCreateInfo info)
         {
             auto mesh = make_ref<CMeshFragment>();
             return mesh;
         }}};

ref_ptr<CMeshFragment> CMeshFactory::create(FMeshCreateInfo info)
{
    auto mesh = m_mFactory[info.eType](info);
    CMeshLoader::load(info.srSrc, mesh, info.bUseIncludedMaterial);

    if (!info.bUseIncludedMaterial)
    {
        /*for (auto &matInfo : info.vMaterials)
        {
            ref_ptr<Material::CMaterialBase> material = Material::CMaterialFactory::create(matInfo);
            CResourceManager::inst()->addExisting(matInfo.srName, material);
            mesh->setMaterial(material);
        }*/
    }
    return mesh;
}
