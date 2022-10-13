#pragma once
#include "SceneBase.h"
#include "util/helpers.hpp"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CSceneManager
            {
            public:
                void load(const std::string &scene_name);
                utl::scope_ptr<CSceneBase>& getScene() { return pCurrentScene; }

            private:
                utl::scope_ptr<CSceneBase> pCurrentScene;
            };
        }
    }
}