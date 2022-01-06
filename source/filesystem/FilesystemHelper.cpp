#include "FilesystemHelper.h"
#include <codecvt>

namespace Engine
{
    std::vector<char> FilesystemHelper::ReadFile(const std::string& srPath)
    {
        std::ifstream file(srPath, std::ios_base::in | std::ios_base::binary);
        file.imbue(std::locale(std::locale::empty(), new std::codecvt<char16_t, char, std::mbstate_t>));
        return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    bool FilesystemHelper::IsFileExist(const std::string& srPath)
    {
        return fs::exists(srPath);
    }
}