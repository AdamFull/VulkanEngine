#pragma once

namespace Engine
{
    struct FilesystemHelper
    {
        static std::string readFile(const fs::path& srPAth);
        static bool isFileExist(const fs::path& srPath);
        static fs::path getWorkDir();
        
        template<class T>
        static T getConfigAs(const fs::path& srPath)
        {
            auto full_path = getWorkDir() / fs::weakly_canonical(srPath);
            auto res_json = nlohmann::json::parse(readFile(full_path));
            T data;
            res_json.get_to(data);
            return data;
        }
    };
}