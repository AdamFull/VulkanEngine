#pragma once
#include "chaiscript/chaiscript.hpp"

namespace engine
{
    namespace scripting
    {
        namespace modules
        {
            static std::pair<const std::string, chaiscript::ModulePtr> vulkan()
            {
                auto pModule = chaiscript::ModulePtr(new chaiscript::Module());

                return std::make_pair("vulkan", pModule);
            }
        }
    }
}