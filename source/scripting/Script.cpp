#include "Script.h"

using namespace engine::scripting;

int32_t CScript::resolve(chaiscript::Boxed_Value& boxedValue)
{
    auto type_name = boxedValue.get_type_info().name();
    if(type_name == "bool")
        return static_cast<int32_t>(executor.boxed_cast<bool>(boxedValue));
    else if(type_name == "int")
        return executor.boxed_cast<int32_t>(boxedValue);
    else if(type_name == "unsigned int")
        return static_cast<int32_t>(executor.boxed_cast<uint32_t>(boxedValue));
    else if(type_name == "long")
        return static_cast<int32_t>(executor.boxed_cast<long>(boxedValue));
    else if(type_name == "float")
        return static_cast<int32_t>(executor.boxed_cast<float>(boxedValue));
    else if(type_name == "double")
        return static_cast<int32_t>(executor.boxed_cast<double>(boxedValue));
    return -1;
}