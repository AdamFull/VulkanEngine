#pragma once
#include "serializer/Serialization.hpp"

namespace Engine
{
    struct FilesystemHelper
    {
        /**
         * @brief Reads files relatively assets directory
         * 
         * @param srPAth Local path
         * @return std::string Readed file data
         */
        static std::string readFile(const fs::path& srPAth);

        static void writeFile(const fs::path& srPath, const std::string& data)
        {
            std::ofstream file(getWorkDir() / srPath, std::ios::out | std::ios::binary);
            file << data;
            file.close();
        }

        /**
         * @brief Writes data relatively assets directory
         * 
         * @param srPath File path 
         * @param binary Binary data
         */
        static void writeFile(const fs::path& srPath, std::vector<uint8_t>& binary)
        {
            std::ofstream file(getWorkDir() / srPath, std::ios::out | std::ios::binary);
            file.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
            file.close();
        }

        /**
         * @brief Checks is file exists relatively assets directory
         * 
         * @param srPath File path
         * @return true when file exists
         */
        static bool isFileExist(const fs::path& srPath);

        /**
         * @brief Returns assets folder path
         * 
         * @return fs::path Assets path
         */
        static fs::path getWorkDir();
        
        /**
         * @brief Reads json config in to class T
         * 
         * @tparam T Data type
         * @param srPath Path to file
         * @return T Object with data type T
         */
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