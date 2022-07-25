#pragma once
#include "SceneComponent.h"
#include "scripting/Script.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CScriptingComponent : public CSceneComponent
            {
            public:
                virtual ~CScriptingComponent();

                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
                void update(float fDeltaTime) override;

                void addScript(const std::string& srScriptPath);
            private:
                // TODO: add name
                std::vector<utl::ref_ptr<scripting::CScript>> vScripts;
            };
        }
    }
}