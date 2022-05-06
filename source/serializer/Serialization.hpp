#pragma once
#include <external/tinygltf/json.hpp>

template<class T>
void ParseArgument(const nlohmann::json &json, T &type, const std::string& tag, bool required = false)
{
    if (json.find(tag) != json.end())
    {
        json.at(tag).get_to(type);
        return;
    }

    if (required)
    {
        std::stringstream ss;
        ss << "Tag named \"" << tag << "\" is required, but not found!";
        throw std::runtime_error(ss.str());
    }
}

namespace nlohmann
{
    template <typename _T>
    struct adl_serializer<std::optional<_T>> 
    {
        static void to_json(json& j, const std::optional<_T>& opt) 
        {
            if (!opt.has_value())
                j = nullptr;
            else
                j = opt.value();
        }

        static void from_json(const json& j, std::optional<_T>& opt) 
        {
            if (j.is_null())
                opt = std::nullopt;
            else
                opt = j.get<_T>();
        }
    };
}


#define REGISTER_SERIALIZATION_BLOCK_H(TYPE_REGISTER) \
void to_json(nlohmann::json &json, const TYPE_REGISTER &type); \
void from_json(const nlohmann::json &json, TYPE_REGISTER &type);