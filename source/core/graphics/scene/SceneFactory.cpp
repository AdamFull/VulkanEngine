#include "SceneFactory.h"
#include "graphics/VulkanHighLevel.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include "resources/ResourceManager.h"

#include "RenderScene.h"
#include "VoxelScene.h"

using namespace engine;
using namespace engine::resources;
using namespace engine::core::scene;
using namespace resources::material;
using namespace resources::loaders;

utl::scope_ptr<CSceneBase> CSceneFactory::create(std::string srScenePath)
{
    FSceneCreateInfo info = FilesystemHelper::getConfigAs<FSceneCreateInfo>(srScenePath);
    return constructScene(info);
}

utl::scope_ptr<CSceneBase> CSceneFactory::constructScene(const FSceneCreateInfo& info)
{
    utl::scope_ptr<CSceneBase> pRenderScene;

    switch(info.eType)
    {
    case ESceneType::eDefault: 
    {
        pRenderScene = utl::make_scope<CRenderScene>(); 
        pRenderScene->create();

        auto pScene = static_cast<CRenderScene*>(pRenderScene.get());
        auto pRoot = pScene->getRoot();
        pRoot->setName("root");
        // TODO: check is skybox exists
        pRoot->attach(createNode(info.skybox));

        createNodes(pRoot, info.vSceneObjects);
    } break;

    case ESceneType::eVoxel: 
    {
        pRenderScene = utl::make_scope<CVoxelScene>(); 
        pRenderScene->create();
    }break;
    }
    return pRenderScene;
}

void CSceneFactory::createNodes(utl::ref_ptr<CRenderObject>& pRoot, std::vector<FSceneObject> sceneObjects)
{
    for (auto &object : sceneObjects)
        pRoot->attach(createNode(object));
}

utl::ref_ptr<CRenderObject> CSceneFactory::createNode(FSceneObject info)
{
    utl::ref_ptr<CRenderObject> object = utl::make_ref<CRenderObject>();
    object->setTransform(info.fTransform);
    object->setName(info.srName);

    for(auto& component : info.components)
    {
        std::visit([&](auto&& val)
        {
            using _Ty = std::decay_t<decltype(val)>;
            createComponent<_Ty>(object, val);
        }, component);
    }

    if (!info.vSceneObjects.empty())
        createNodes(object, info.vSceneObjects);

    return object;
}

void CSceneFactory::addCamera(utl::ref_ptr<CRenderObject>& pNode)
{
    UCamera->attach(pNode);
}

void CSceneFactory::addTexture(const std::string& srName, utl::ref_ptr<CImage>& pTexture)
{
    UResources->addExisting(srName, pTexture);
}

void CSceneFactory::addMaterial(utl::ref_ptr<CMaterialBase>& pMaterial)
{
    UResources->addExisting(pMaterial->getName(), pMaterial);
}

void CSceneFactory::addLight(utl::ref_ptr<core::scene::CRenderObject>& pNode)
{
    ULightning->addLight(pNode);
}