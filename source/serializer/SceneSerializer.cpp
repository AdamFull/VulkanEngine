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
        auto res_json = nlohmann::json::parse(input);
        auto res_arr = res_json.front();
        std::vector<FSceneInitializer> vResInit;
        for(auto& res : res_arr)
        {
            FSceneInitializer fresinit;
            if(!res["asset"].empty())
                fresinit.srAsset = res.at("asset").get<std::string>();
            if(!res["type"].empty())
                fresinit.srType = res.at("type").get<std::string>();
            if(!res["resources"].empty())
                fresinit.mResources = res.at("resources").get<std::map<std::string, std::string>>();
            vResInit.emplace_back(fresinit);
        }
        return vResInit;
    }
}