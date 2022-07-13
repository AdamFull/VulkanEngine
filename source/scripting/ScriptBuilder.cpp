#include "ScriptBuilder.h"
#include "modules/glm_module.hpp"

using namespace engine::scripting;

template<>
std::unique_ptr<CScriptBuilder> utl::singleton<CScriptBuilder>::_instance{ nullptr };

std::shared_ptr<CScript> CScriptBuilder::build(const std::string& srProgramm)
{
    auto pScript = std::make_shared<CScript>(srProgramm);
    applyBindings(pScript);
    return pScript;
}

void CScriptBuilder::initialize()
{
    m_mModules.emplace("glm", modules::glm());
    m_bInitialized = true;
}

void CScriptBuilder::applyBindings(std::shared_ptr<CScript>& pScript)
{
    if (!m_bInitialized)
        initialize();

    pScript->bind(chaiscript::fun(
    [=](const std::string& namespace_name) 
    {
        pScript->bind(m_mModules[namespace_name]);
    }), "include"); 
}