#include "SceneManager.h"
#include "SceneFactory.h"

using namespace Engine::Core::Scene;

template<>
scope_ptr<CSceneManager> utl::singleton<CSceneManager>::_instance{nullptr};

void CSceneManager::load(const std::string& scene_name)
{
    pCurrentScene = CSceneFactory::create(scene_name);
}

void CSceneManager::unload()
{
    pCurrentScene->cleanup();
}