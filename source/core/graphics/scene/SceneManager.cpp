#include "SceneManager.h"
#include "SceneFactory.h"

using namespace engine::core::scene;

void CSceneManager::load(const std::string& scene_name)
{
    pCurrentScene = CSceneFactory::create(scene_name);
}