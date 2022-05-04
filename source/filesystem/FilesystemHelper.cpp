#include "FilesystemHelper.h"
#include <codecvt>

namespace Engine
{
    std::string FilesystemHelper::readFile(const fs::path& srPath)
    {
        std::ifstream file(getWorkDir() / srPath, std::ios_base::in | std::ios_base::binary);
        file.rdbuf()->pubsetbuf(0, 0);
        file.imbue(std::locale(std::locale::empty(), new std::codecvt<char16_t, char, std::mbstate_t>));
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    bool FilesystemHelper::isFileExist(const fs::path& srPath)
    {
        return fs::exists(srPath);
    }

    fs::path FilesystemHelper::getWorkDir()
    {
        return fs::current_path().assign(fs::weakly_canonical("../../assets/"));
    }
}