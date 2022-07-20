#include "VulkanHighLevel.h"
#include "VulkanStaticHelper.h"
#include <glslang/Public/ShaderLang.h>
#include "VulkanDevice.hpp"
#include "editor/EditorUI.h"

using namespace engine::resources;
using namespace engine::editor;
using namespace engine::core;
using namespace engine::core::render;
using namespace engine::core::window;
using namespace engine::core::scene;

template<>
utl::scope_ptr<CVulkanHighLevel> utl::singleton<CVulkanHighLevel>::_instance{nullptr};

CVulkanHighLevel::~CVulkanHighLevel()
{
    pDevice->GPUWait();
}

//TODO: remove this class
void CVulkanHighLevel::create(FEngineCreateInfo createInfo)
{
    ci = createInfo;
    pWindow = utl::make_scope<CWindowHandle>(createInfo.window);
    pDevice = utl::make_scope<CDevice>(createInfo.device);
    pRender = utl::make_scope<CRenderSystem>();

    pThreadPool = utl::make_scope<CThreadPool>();
    pVBO = utl::make_scope<CVertexBufferObject>();
    pCameraManager = utl::make_scope<CCameraManager>();
    pLightManager = utl::make_scope<CLightSourceManager>();

    pEditor = utl::make_scope<CEditor>();
    pEditorUI = utl::make_scope<CEditorUI>();

    pResources = utl::make_scope<CResourceManager>();
    pScene = utl::make_scope<CSceneManager>();
    pScene->load(createInfo.engine.scene);

    if (!glslang::InitializeProcess())
		throw std::runtime_error("Failed to initialize glslang processor.");

    pRender->create();
}

namespace engine
{
    namespace core
    {
        NLOHMANN_JSON_SERIALIZE_ENUM
        (
            ELaunchMode,
            {
                {ELaunchMode::eGame, "game"},
                {ELaunchMode::eEditor, "editor"}
            }
        )
        void to_json(nlohmann::json &json, const FEngineModeCreateInfo &type)
        {
            json = nlohmann::json
            {
                {"mode", type.mode},
                {"start_scene", type.scene}
            };
        }

        void from_json(const nlohmann::json &json, FEngineModeCreateInfo &type)
        {
            ParseArgument(json, type.mode, "mode", true);
            ParseArgument(json, type.scene, "start_scene", true);
        }

        void to_json(nlohmann::json &json, const FEngineCreateInfo &type)
        {
            json = nlohmann::json
            {
                {"window", type.window},
                {"device", type.device},
                {"engine", type.engine}
            };
        }

        void from_json(const nlohmann::json &json, FEngineCreateInfo &type)
        {
            ParseArgument(json, type.window, "window", true);
            ParseArgument(json, type.device, "device", true);
            ParseArgument(json, type.engine, "engine", true);
        }
    }
}