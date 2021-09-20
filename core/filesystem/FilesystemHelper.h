#pragma once

namespace Engine
{
    struct FilesystemHelper
    {
        static std::vector<char> ReadFile(const std::string& srPAth);
        static bool IsFileExist(const std::string& srPath);
    };
}