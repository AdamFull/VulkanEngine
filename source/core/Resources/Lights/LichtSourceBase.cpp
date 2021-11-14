#include "LightSourceBase.h"

using namespace Engine::Resources;
using namespace Engine::Resources::Light;

void LightSourceBase::Create(std::shared_ptr<ResourceManager> pResMgr)
{
    ResourceBase::Create(pResMgr);
}

void LightSourceBase::Update(uint32_t imageIndex)
{
    ResourceBase::Update(imageIndex);
}
