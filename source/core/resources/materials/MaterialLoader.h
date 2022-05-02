#pragma once
#include "graphics/pipeline/Shader.h"

namespace Engine
{
    namespace Resources
    {
        struct FMaterialInfo
        {
            struct FCreationInfo
            {
                vk::CullModeFlagBits culling{vk::CullModeFlagBits::eNone};
                vk::FrontFace frontface{vk::FrontFace::eCounterClockwise};
                vk::Bool32 enableDepth{VK_FALSE};
                std::vector<vk::DynamicState> dynamicStateEnables;
                std::vector<std::string> stages;
                std::vector<Core::Pipeline::CShader::Define> defines;
            };
            std::unordered_map<std::string, FCreationInfo> creationInfo;
        };

        void to_json(nlohmann::json &json, const FMaterialInfo::FCreationInfo &type);
        void from_json(const nlohmann::json &json, FMaterialInfo::FCreationInfo &type);

        void to_json(nlohmann::json &json, const FMaterialInfo &type);
        void from_json(const nlohmann::json &json, FMaterialInfo &type);

        class CMaterialLoader : public utl::singleton<CMaterialLoader>
        {
        public:
            CMaterialLoader();
            ~CMaterialLoader();
        private:
            void load();
            void save();

            FMaterialInfo data;
        };
    }
}