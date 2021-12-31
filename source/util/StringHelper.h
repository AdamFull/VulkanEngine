#pragma once

struct StringHelper
{
    static std::vector<std::string> Split(const std::string &str, char sep);
    static std::string ReplaceFirst(std::string str, std::string_view token, std::string_view to);
};
