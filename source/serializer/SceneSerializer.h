#pragma once

namespace Engine
{
    struct FSceneInitializer
    {
        std::string srName;
        std::string srType;
        std::map<std::string, std::string> mResources;
    };

    struct SceneSerializer
    {
        static std::vector<char> Serialize(std::vector<FSceneInitializer>& input);
        static std::vector<FSceneInitializer> DeSerialize(std::vector<char>& input);
    };
}