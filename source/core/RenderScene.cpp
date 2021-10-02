#include "RenderScene.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void RenderScene::Create()
    {
        m_pRoot = std::make_shared<SceneRootComponent>();
    }

    void RenderScene::Destroy()
    {
        UDevice->GPUWait();
        if(URenderer->GetFrameStartFlag())
        {
            bool bResult;
            auto commandBuffer = URenderer->GetCurrentCommandBuffer();
            UHLInstance->EndFrame(commandBuffer, &bResult);
        }

        m_pRoot->Destroy();
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
        {
            m_pRoot->Cleanup();
            m_pRoot->ReCreate();
        }

        uint32_t currentFrame = URenderer->GetImageIndex();
        auto camera = CameraManager::GetInstance()->GetCurrentCamera();

        /*FUniformData ubo{};
        ubo.transform = camera->GetProjection() * camera->GetView();*/

        UHLInstance->BeginRender(commandBuffer);

        m_pRoot->Render(commandBuffer, currentFrame);

        UHLInstance->EndRender(commandBuffer);

        UHLInstance->EndFrame(commandBuffer, &bResult);
        if(!bResult)
        {
            m_pRoot->Cleanup();
            m_pRoot->ReCreate();
        }
    }
}