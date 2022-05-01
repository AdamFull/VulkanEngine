#include "ShaderCache.h"

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
                    {"sizes", type.localSizes}
                };
            }

            void from_json(const nlohmann::json &json, FShaderCache::FCachedShader &type)
            {
                ParseArgument(json, type.shaderStage, "stage", true);
                ParseArgument(json, type.shaderCode, "code", true);
                ParseArgument(json, type.localSizes, "sizes", true);
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

CShaderCache::~CShaderCache()
{
    save();
}

void CShaderCache::add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, std::array<std::optional<uint32_t>, 3>& localSizes)
{
    //add file data hashing for check difference
    auto it = cacheDTO.cache.find(name);
    if(it != cacheDTO.cache.end())
    {
        if(code != it->second.shaderCode)
            update(name, code, localSizes);
        return;
    }
    
    cacheDTO.cache.emplace(name, FShaderCache::FCachedShader{stage, code, localSizes});
    save();
}

void CShaderCache::update(const std::string& name, const std::vector<uint32_t>& code, std::array<std::optional<uint32_t>, 3>& localSizes)
{
    cacheDTO.cache[name].shaderCode = code;
    save();
}

std::optional<FShaderCache::FCachedShader> CShaderCache::get(const std::string& name)
{   
    auto it = cacheDTO.cache.find(name);
    if(it != cacheDTO.cache.end())
        return it->second;
    return std::nullopt;
}


void CShaderCache::load()
{
    std::ifstream infile("spirv.cache", std::ios::in | std::ios::binary);
    infile.rdbuf()->pubsetbuf(0, 0);
    auto tmp = std::string(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
    if(!tmp.empty())
    {
        auto bson = nlohmann::json::from_bson(tmp);
        bson.get_to(cacheDTO);
    }
    infile.close();
}

void CShaderCache::save()
{
    std::ofstream outfile("spirv.cache", std::ios::out | std::ios::binary);
    auto binary = nlohmann::json::to_bson(cacheDTO);
    outfile.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
    outfile.close();
}
