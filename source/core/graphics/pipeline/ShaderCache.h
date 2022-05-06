#pragma once
#include <util/helpers.hpp>
#include "serializer/Serialization.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            struct FShaderCache
            {
                struct FCachedShader
                {
                    vk::ShaderStageFlagBits shaderStage;
                    std::vector<uint32_t> shaderCode;
                    std::array<std::optional<uint32_t>, 3> localSizes;
                    std::string hash{};

                    bool operator==(const FCachedShader& rhs) const
                    {
                        return this->shaderStage == rhs.shaderStage && this->shaderCode == rhs.shaderCode;
                    }

                    bool operator!=(const FCachedShader& rhs) const
                    {
                        return this->shaderStage != rhs.shaderStage || this->shaderCode != rhs.shaderCode;
                    }
                };

                std::map<std::string, FCachedShader> cache;
            };

            void to_json(nlohmann::json &json, const FShaderCache::FCachedShader &type);
            void from_json(const nlohmann::json &json, FShaderCache::FCachedShader &type);

            void to_json(nlohmann::json &json, const FShaderCache &type);
            void from_json(const nlohmann::json &json, FShaderCache &type);

            class CShaderCache : public utl::singleton<CShaderCache>
            {
            public:
                CShaderCache();
                void add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, const std::string& moduleCode, std::array<std::optional<uint32_t>, 3>& localSizes);
                void update(const std::string& name, const std::vector<uint32_t>& code, std::array<std::optional<uint32_t>, 3>& localSizes, const std::string& hash);
                std::optional<FShaderCache::FCachedShader> get(const std::string& name, const std::string& moduleCode);
            private:
                void load();
                void save();
                std::hash<std::string> hasher;
                FShaderCache cacheDTO;
            };
        }
    }
}