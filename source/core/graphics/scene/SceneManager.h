#pragma once
#include "RenderScene.h"
#include "util/helpers.hpp"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CSceneManager : public utl::singleton<CSceneManager>
            {
            public:
                void load(const std::string &scene_name);
                void unload();
                utl::scope_ptr<CRenderScene> &getScene() { return pCurrentScene; }

            private:
                utl::scope_ptr<CRenderScene> pCurrentScene;
            };
        }
    }
}