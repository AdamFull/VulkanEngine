#pragma once
#include "RenderScene.h"
#include "util/helpers.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CSceneManager : public utl::singleton<CSceneManager>
    {
    public:
        void load(const std::string &scene_name);
        void unload();
        std::unique_ptr<CRenderScene> &getScene() { return pCurrentScene; }

    private:
        std::unique_ptr<CRenderScene> pCurrentScene;
    };
        }
    }
}