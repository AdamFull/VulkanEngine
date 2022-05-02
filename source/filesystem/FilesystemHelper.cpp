#include "FilesystemHelper.h"
#include <codecvt>

namespace Engine
{
    std::string FilesystemHelper::ReadFile(const fs::path& srPath)
    {
        std::ifstream file(srPath, std::ios_base::in | std::ios_base::binary);
        file.imbue(std::locale(std::locale::empty(), new std::codecvt<char16_t, char, std::mbstate_t>));
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    bool FilesystemHelper::IsFileExist(const fs::path& srPath)
    {
        return fs::exists(srPath);
    }

    fs::path FilesystemHelper::GetWorkDir()
    {
        return fs::current_path().assign(fs::weakly_canonical("../../assets/"));
    }
}