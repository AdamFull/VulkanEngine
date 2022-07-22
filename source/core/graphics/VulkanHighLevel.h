#pragma once
#include "util/threading.hpp"
#include "Window/WindowHandle.h"
#include "VulkanDevice.hpp"
#include "VulkanStaticHelper.h"
#include "buffer/VulkanBuffer.h"
#include "VulkanRenderSystem.h"
#include "scene/SceneManager.h"
#include "editor/Editor.h"
#include "editor/EditorUI.h"
#include "serializer/Serialization.hpp"
#include "buffer/VulkanVBO.h"
#include "scene/objects/components/CameraManager.h"
#include "scene/objects/components/LightSourceManager.h"

#include "pipeline/Pipeline.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"

#define UHLInstance engine::core::CVulkanHighLevel::inst()
#define UWindow UHLInstance->getWindow()
#define UDevice UHLInstance->getDevice()
#define UVBO UHLInstance->getVBO()
#define URenderer UHLInstance->getRenderer()
#define UCamera UHLInstance->getCameraManager()
#define ULightning UHLInstance->getLightManager()
#define UScene UHLInstance->getScene()
#define UResources UHLInstance->getResources()
#define UEditor UHLInstance->getEditor()
#define UEditorUI UHLInstance->getEditorUI()
#define UThreadPool UHLInstance->getThreadPool()

namespace engine
{
    namespace core
    {
        enum class ELaunchMode
        {
            eNone,
            eGame,
            eEditor
        };

        struct FEngineModeCreateInfo
        {
            ELaunchMode mode{ELaunchMode::eGame};
            std::string scene;
        };

        struct FEngineCreateInfo
        {
            window::FWindowCreateInfo window;
            FDeviceCreateInfo device;
            FEngineModeCreateInfo engine;
        };

        class CVulkanHighLevel : public utl::singleton<CVulkanHighLevel>
        {
        public:
            ~CVulkanHighLevel();
            void create(FEngineCreateInfo createInfo);

            const FEngineCreateInfo &getCI() const { return ci; }

            std::unique_ptr<window::CWindowHandle>& getWindow() { return pWindow; }
            std::unique_ptr<CDevice>& getDevice() { return pDevice; }
            std::unique_ptr<CVertexBufferObject>& getVBO() { return pVBO; }
            std::unique_ptr<render::CRenderSystem>& getRenderer() { return pRender; }
            std::unique_ptr<scene::CCameraManager>& getCameraManager() { return pCameraManager; }
            std::unique_ptr<scene::CLightSourceManager>& getLightManager() { return pLightManager; }
            std::unique_ptr<scene::CSceneManager>& getScene() { return pScene; }
            std::unique_ptr<resources::CResourceManager>& getResources() { return pResources; }
            std::unique_ptr<editor::CEditor>& getEditor() { return pEditor; }
            std::unique_ptr<editor::CEditorUI>& getEditorUI() { return pEditorUI; }
            std::unique_ptr<CThreadPool>& getThreadPool() { return pThreadPool; }


        private:
            FEngineCreateInfo ci{};

            std::unique_ptr<window::CWindowHandle> pWindow;
            std::unique_ptr<CDevice> pDevice;
            std::unique_ptr<CVertexBufferObject> pVBO;
            std::unique_ptr<render::CRenderSystem> pRender;
            std::unique_ptr<scene::CCameraManager> pCameraManager;
            std::unique_ptr<scene::CLightSourceManager> pLightManager;
            std::unique_ptr<scene::CSceneManager> pScene;
            std::unique_ptr<resources::CResourceManager> pResources;
            std::unique_ptr<editor::CEditor> pEditor;
            std::unique_ptr<editor::CEditorUI> pEditorUI;
            std::unique_ptr<CThreadPool> pThreadPool;
        };

        REGISTER_SERIALIZATION_BLOCK_H(FEngineModeCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FEngineCreateInfo);
    }
}
