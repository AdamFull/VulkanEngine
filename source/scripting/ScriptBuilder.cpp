#include "ScriptBuilder.h"
#include "filesystem/FilesystemHelper.h"

#include "modules/glm_module.hpp"
#include "modules/vulkan_module.hpp"
#include "modules/engine.hpp"

using namespace engine;
using namespace engine::scripting;

template<>
std::unique_ptr<CScriptBuilder> utl::singleton<CScriptBuilder>::_instance{ nullptr };

utl::ref_ptr<CScript> CScriptBuilder::build(const std::string& srProgramm)
{
    auto pScript = utl::make_ref<CScript>();
    auto srData = FilesystemHelper::readFile(srProgramm);
    pScript->vIncludes.emplace_back(std::make_pair(srProgramm, srData));
    applyBindings(pScript);
    return pScript;
}

void CScriptBuilder::initialize()
{
    m_mModules.emplace(modules::glm());
    m_mModules.emplace(modules::vulkan());
    m_mModules.emplace(modules::engine_structs());
    m_mModules.emplace(modules::engine_objects());
    m_bInitialized = true;
}

void CScriptBuilder::applyBindings(utl::ref_ptr<CScript>& pScript)
{
    if (!m_bInitialized)
        initialize();

    pScript->bind(chaiscript::fun(
    [=](const std::string& namespace_name) 
    {
        if (fs::path(namespace_name).extension() == ".chai")
        {
            auto srData = FilesystemHelper::readFile(namespace_name);
            if (!srData.empty())
            {
                pScript->execute<void>(srData, namespace_name);
                return;
            }
            else
            {
                std::stringstream ss;
                ss << "Cannot include file \"" << namespace_name << "\". File not found.";
                throw std::runtime_error(ss.str());
            }
        }

        pScript->bind(m_mModules[namespace_name]);
    }), "include"); 
}