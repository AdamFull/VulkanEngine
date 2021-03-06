#include "ScriptBuilder.h"
#include "modules/glm_module.hpp"
#include "filesystem/FilesystemHelper.h"

using namespace engine;
using namespace engine::scripting;

template<>
std::unique_ptr<CScriptBuilder> utl::singleton<CScriptBuilder>::_instance{ nullptr };

std::shared_ptr<CScript> CScriptBuilder::build(const std::string& srProgramm)
{
    auto pScript = std::make_shared<CScript>();
    auto srData = FilesystemHelper::readFile(srProgramm);
    pScript->vIncludes.emplace_back(std::make_pair(srProgramm, srData));
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