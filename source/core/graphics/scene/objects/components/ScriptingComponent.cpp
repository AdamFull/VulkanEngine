#include "ScriptingComponent.h"
#include "scripting/ScriptBuilder.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace engine;
using namespace engine::core;
using namespace engine::core::scene;
using namespace engine::scripting;

CScriptingComponent::~CScriptingComponent()
{
    vScripts.clear();
}

void CScriptingComponent::create()
{
    CSceneComponent::create();

    for(auto& script : vScripts)
    {
        script->bind_global(chaiscript::var(pParent.lock().get()), "this");
        script->bind_global(chaiscript::var(&this->componentTick), "componentTick");
        script->call<void>("create()");
    }
}

void CScriptingComponent::reCreate()
{
    CSceneComponent::reCreate();

    for(auto& script : vScripts)
        script->call<void>("recreate()");
}

void CScriptingComponent::render(vk::CommandBuffer &commandBuffer)
{
    CSceneComponent::render(commandBuffer);

    for(auto& script : vScripts)
        script->call<void>("render()");
}

void CScriptingComponent::update(float fDeltaTime)
{
    CSceneComponent::update(fDeltaTime);

    for(auto& script : vScripts)
        script->call<void>("update()");
}

void CScriptingComponent::addScript(const std::string& srScriptPath)
{
    vScripts.emplace_back(CScriptBuilder::inst()->build(srScriptPath));
}
