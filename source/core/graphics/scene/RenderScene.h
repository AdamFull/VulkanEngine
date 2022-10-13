#pragma once
#include "SceneBase.h"
#include "Resources/ResourceManager.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CRenderScene : public CSceneBase
            {
            public:
                CRenderScene() = default;
                
                void create() override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render() override;

                void createObjects() override;
                ESceneType getType() override { return ESceneType::eDefault; }
            };
        }
    }
}