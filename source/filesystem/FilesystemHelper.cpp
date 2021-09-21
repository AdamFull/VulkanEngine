#include "FilesystemHelper.h"

namespace Engine
{
    std::vector<char> FilesystemHelper::ReadFile(const std::string& srPath)
    {
        std::ifstream file(srPath, std::ios_base::in | std::ios_base::binary);
        return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    bool FilesystemHelper::IsFileExist(const std::string& srPath)
    {
        return true;
    }
}