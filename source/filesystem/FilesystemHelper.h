#pragma once

namespace Engine
{
    struct FilesystemHelper
    {
        static std::string readFile(const fs::path& srPAth);

        static void writeFile(const fs::path& srPath, const std::string& data)
        {
            std::ofstream file(getWorkDir() / srPath, std::ios::out | std::ios::binary);
            file << data;
            file.close();
        }

        static void writeFile(const fs::path& srPath, std::vector<uint8_t>& binary)
        {
            std::ofstream file(getWorkDir() / srPath, std::ios::out | std::ios::binary);
            file.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
            file.close();
        }

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