#include "SceneComponent.h"

using namespace engine::core::scene;

void CSceneComponent::create()
{

}

void CSceneComponent::reCreate()
{

}

void CSceneComponent::render(vk::CommandBuffer &commandBuffer)
{

}

void CSceneComponent::update(float fDeltaTime)
{
    componentTick += fDeltaTime * 0.025f;
}

void CSceneComponent::cleanup()
{

}

void CSceneComponent::destroy()
{

}
