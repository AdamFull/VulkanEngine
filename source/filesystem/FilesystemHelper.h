#pragma once

namespace Engine
{
    struct FilesystemHelper
    {
        static std::vector<char> ReadFile(const fs::path& srPAth);
        static bool IsFileExist(const fs::path& srPath);
        static fs::path GetWorkDir();
        
        template<class T>
        static T GetConfigAs(const fs::path& srPath)
        {
            auto full_path = GetWorkDir() / fs::weakly_canonical(srPath);
            auto res_json = nlohmann::json::parse(ReadFile(full_path));
            T data;
            res_json.get_to(data);
            return data;
        }
    };
}