#include "RenderScene.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void RenderScene::Create()
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
    }

    void RenderScene::AttachObject(std::shared_ptr<RenderObject> object)
    {
        object->SetParent(m_pRoot);
    }

    void RenderScene::CreateObjects()
    {
        m_pRoot->Create();
    }

    std::shared_ptr<RenderObject> RenderScene::FindSceneObject(std::string srName)
    {
        return m_pRoot->Find(srName);
    }

    void RenderScene::Render(float fDeltaTime)
    {
        m_pRoot->Update(fDeltaTime);

        bool bResult;
        auto commandBuffer = UHLInstance->BeginFrame(&bResult);
        if(!bResult)
            m_pRoot->ReCreate();

        uint32_t currentFrame = URenderer->GetImageIndex();
        m_pRoot->Render(commandBuffer, currentFrame);

        UHLInstance->EndFrame(commandBuffer, &bResult);
        if(!bResult)
            m_pRoot->ReCreate();
    }
}