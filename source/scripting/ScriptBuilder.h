#pragma once
#include "Script.h"

namespace engine
{
    namespace scripting
    {
        class CScriptBuilder : public utl::singleton<CScriptBuilder>
        {
        protected:
            
        public:
            CScriptBuilder() = default;
            std::shared_ptr<CScript> build(const std::string& srProgramm);
        private:
            bool m_bInitialized{ false };
            std::map<std::string, chaiscript::ModulePtr> m_mModules;
        private:
            void initialize();
            void applyBindings(std::shared_ptr<CScript>& pScript);
        };
    }
}