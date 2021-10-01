#pragma once

namespace Engine
{
    struct FResourceInitializer
    {
        std::string srName;
        std::string srType;
        std::string srPath;
        std::map<std::string, std::string> mResources;
    };

    struct ResourceSerializer
    {
        static std::vector<char> Serialize(std::vector<FResourceInitializer>& input);
        static std::vector<FResourceInitializer> DeSerialize(std::vector<char>& input);
    };
    
}