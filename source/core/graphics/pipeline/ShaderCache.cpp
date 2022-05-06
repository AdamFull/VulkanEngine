#include "ShaderCache.h"
#include "filesystem/FilesystemHelper.h"

namespace vk
{
    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        ShaderStageFlagBits,
        {
            {ShaderStageFlagBits::eVertex, "vert"},
            {ShaderStageFlagBits::eTessellationEvaluation, "tese"},
            {ShaderStageFlagBits::eTessellationControl, "tesc"},
            {ShaderStageFlagBits::eTaskNV, "tnv"},
            {ShaderStageFlagBits::eSubpassShadingHUAWEI, "ssh"},
            {ShaderStageFlagBits::eRaygenNV, "rnv"},
            {ShaderStageFlagBits::eRaygenKHR, "rkhr"},
            {ShaderStageFlagBits::eMissNV, "msnv"},
            {ShaderStageFlagBits::eMissKHR, "mskhr"},
            {ShaderStageFlagBits::eMeshNV, "mnv"},
            {ShaderStageFlagBits::eIntersectionNV, "inv"},
            {ShaderStageFlagBits::eIntersectionKHR, "ikhr"},
            {ShaderStageFlagBits::eGeometry, "geom"},
            {ShaderStageFlagBits::eFragment, "frag"},
            {ShaderStageFlagBits::eCompute, "comp"},
            {ShaderStageFlagBits::eClosestHitNV, "chnv"},
            {ShaderStageFlagBits::eClosestHitKHR, "chkhr"},
            {ShaderStageFlagBits::eCallableNV, "cnv"},
            {ShaderStageFlagBits::eCallableKHR, "ckhr"},
            {ShaderStageFlagBits::eAnyHitNV, "ahnv"},
            {ShaderStageFlagBits::eAnyHitKHR, "ahkhr"},
            {ShaderStageFlagBits::eAllGraphics, "ag"},
            {ShaderStageFlagBits::eAll, "a"}
        }
    )
}

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            void to_json(nlohmann::json &json, const FShaderCache::FCachedShader &type)
            {
                json = nlohmann::json
                {
                    {"stage", type.shaderStage},
                    {"code", type.shaderCode},
                    {"sizes", type.localSizes},
                    {"hash", type.hash}
                };
            }

            void from_json(const nlohmann::json &json, FShaderCache::FCachedShader &type)
            {
                ParseArgument(json, type.shaderStage, "stage", true);
                ParseArgument(json, type.shaderCode, "code", true);
                ParseArgument(json, type.localSizes, "sizes", true);
                ParseArgument(json, type.hash, "hash", true);
            }


            void to_json(nlohmann::json &json, const FShaderCache &type)
            {
                json = nlohmann::json
                {
                    {"cache", type.cache}
                };
            }

            void from_json(const nlohmann::json &json, FShaderCache &type)
            {
                ParseArgument(json, type.cache, "cache", true);
            }
        }
    }
}

using namespace Engine::Core::Pipeline;

template<>
std::unique_ptr<CShaderCache> utl::singleton<CShaderCache>::_instance{nullptr};

CShaderCache::CShaderCache()
{
    load();
}

void CShaderCache::add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, const std::string& moduleCode, std::array<std::optional<uint32_t>, 3>& localSizes)
{
    auto hash = std::to_string(hasher(moduleCode));
    auto it = cacheDTO.cache.find(name);
    if(it != cacheDTO.cache.end())
    {
        if(hash != it->second.hash)
            update(name, code, localSizes, hash);
        return;
    }
    
    cacheDTO.cache.emplace(name, FShaderCache::FCachedShader{stage, code, localSizes, hash});
    save();
}

void CShaderCache::update(const std::string& name, const std::vector<uint32_t>& code, std::array<std::optional<uint32_t>, 3>& localSizes, const std::string& hash)
{
    cacheDTO.cache[name].shaderCode = code;
    cacheDTO.cache[name].localSizes = localSizes;
    cacheDTO.cache[name].hash = std::stoull(hash);
    save();
}

std::optional<FShaderCache::FCachedShader> CShaderCache::get(const std::string& name, const std::string& moduleCode)
{   
    auto hash = std::to_string(hasher(moduleCode));
    auto it = cacheDTO.cache.find(name);
    if(it != cacheDTO.cache.end())
    {
        if(hash != it->second.hash)
            return std::nullopt;
        return it->second;
    }
    return std::nullopt;
}


void CShaderCache::load()
{
    auto tmp = FilesystemHelper::readFile("spirv.cache");
    if(!tmp.empty())
    {
        auto bson = nlohmann::json::from_bson(tmp);
        bson.get_to(cacheDTO);
    }
}

void CShaderCache::save()
{
    auto binary = nlohmann::json::to_bson(cacheDTO);
    FilesystemHelper::writeFile("spirv.cache", binary);
}