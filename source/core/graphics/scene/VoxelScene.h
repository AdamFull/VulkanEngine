#pragma once
#include "SceneBase.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CVoxelScene : public CSceneBase
            {
            public:
                CVoxelScene() = default;
                
                void create() override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render() override;

                void createObjects() override;
                ESceneType getType() override { return ESceneType::eVoxel; }
            };
        }
    }
}