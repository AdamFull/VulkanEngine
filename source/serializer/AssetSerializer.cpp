#include "AssetSerializer.h"
#include <nlohmann/json.hpp>

namespace Engine
{
    std::vector<char> AssetSerializer::Serialize(std::vector<FAssetInitializer>& input)
    {
        return std::vector<char>();
    }

    std::vector<FAssetInitializer> AssetSerializer::DeSerialize(std::vector<char>& input)
    {
        auto res_json = nlohmann::json::parse(input);
        auto res_arr = res_json.front();
        std::vector<FAssetInitializer> vResInit;
        for(auto& res : res_arr)
        {
            FAssetInitializer fresinit;
            if(!res["name"].empty())
                fresinit.srName = res.at("name").get<std::string>();
            if(!res["type"].empty())
                fresinit.srType = res.at("type").get<std::string>();
            if(!res["resources"].empty())
                fresinit.mResources = res.at("resources").get<std::map<std::string, std::string>>();
            vResInit.emplace_back(fresinit);
        }
        return vResInit;
    }
}