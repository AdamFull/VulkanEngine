#pragma once

namespace Engine
{
    struct FAssetInitializer
    {
        std::string srName;
        std::string srType;
        std::map<std::string, std::string> mResources;
    };

    struct AssetSerializer
    {
        static std::vector<char> Serialize(std::vector<FAssetInitializer>& input);
        static std::vector<FAssetInitializer> DeSerialize(std::vector<char>& input);
    };
}