#include "SceneSerializer.h"
#include <nlohmann/json.hpp>

namespace Engine
{
    std::vector<char> SceneSerializer::Serialize(std::vector<FSceneInitializer>& input)
    {
        return std::vector<char>();
    }

    std::vector<FSceneInitializer> SceneSerializer::DeSerialize(std::vector<char>& input)
    {

    }
}