#include "ResourceSerializer.h"
#include <nlohmann/json.hpp>

namespace Engine
{
    std::vector<char> ResourceSerializer::Serialize(std::vector<FResourceInitializer>& input)
    {
        return std::vector<char>();
    }

    std::vector<FResourceInitializer> ResourceSerializer::DeSerialize(std::vector<char>& input)
    {
        auto res_json = nlohmann::json::parse(input);
        auto res_arr = res_json.front();
        std::vector<FResourceInitializer> vResInit;
        for(auto& res : res_arr)
        {
            FResourceInitializer fresinit;
            if(!res["name"].empty())
                fresinit.srName = res.at("name").get<std::string>();
            if(!res["type"].empty())
                fresinit.srType = res.at("type").get<std::string>();
            if(!res["path"].empty())
                fresinit.srPath = res.at("path").get<std::string>();
            if(!res["resources"].empty())
                fresinit.mResources = res.at("resources").get<std::map<std::string, std::string>>();
            vResInit.emplace_back(fresinit);
        }
        return vResInit;
    }
}