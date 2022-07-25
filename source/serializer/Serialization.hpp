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
    template <typename _Ty>
    struct adl_serializer<std::optional<_Ty>> 
    {
        static void to_json(json& j, const std::optional<_Ty>& opt) 
        {
            if (!opt.has_value())
                j = nullptr;
            else
                j = opt.value();
        }

        static void from_json(const json& j, std::optional<_Ty>& opt) 
        {
            if (j.is_null())
                opt = std::nullopt;
            else
                opt = j.get<_Ty>();
        }
    };

    // https://github.com/nlohmann/json/issues/1261
    template <typename _Ty, typename... _Args>
    void variant_from_json(const json &j, std::variant<_Args...> &data)
    {
        try {
            data = j.get<_Ty>();
        } catch (...) { }
    }

    template <typename... _Args>
    struct adl_serializer<std::variant<_Args...>>
    {
        static void to_json(json &j, const std::variant<_Args...> &data)
        {
            // Will call j = v automatically for the right type
            std::visit(
                [&j](const auto &v) {
                    j = v;
                },
                data);
        }

        static void from_json(const json &j, std::variant<_Args...> &data)
        {
            // Call variant_from_json for all types, only one will succeed
            (variant_from_json<_Args>(j, data), ...);
        }
    };
}


#define REGISTER_SERIALIZATION_BLOCK_H(TYPE_REGISTER) \
void to_json(nlohmann::json &json, const TYPE_REGISTER &type); \
void from_json(const nlohmann::json &json, TYPE_REGISTER &type);