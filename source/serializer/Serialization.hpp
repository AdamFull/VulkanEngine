#pragma once

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

#define REGISTER_SERIALIZATION_BLOCK_H(TYPE_REGISTER) \
void to_json(nlohmann::json &json, const TYPE_REGISTER &type); \
void from_json(const nlohmann::json &json, TYPE_REGISTER &type);