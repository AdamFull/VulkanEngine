#pragma once
#include "graphics/pipeline/Shader.h"
#include "VulkanMaterial.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            struct FMaterialInfo
            {
                enum class EVertexType
                {
                    eNone,
                    eDefault,
                    eImgui
                };

                struct FCreationInfo
                {
                    EVertexType vertexType;
                    vk::PipelineBindPoint bindPoint;
                    vk::CullModeFlagBits culling{vk::CullModeFlagBits::eNone};
                    vk::FrontFace frontface{vk::FrontFace::eCounterClockwise};
                    vk::Bool32 enableDepth{VK_FALSE};
                    std::vector<vk::DynamicState> dynamicStateEnables{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
                    std::vector<std::string> stages;
                    std::map<std::string, std::string> defines;
                    bool multisampling{false};
                };
                std::unordered_map<std::string, FCreationInfo> creationInfo;
            };

            void to_json(nlohmann::json &json, const FMaterialInfo::FCreationInfo &type);
            void from_json(const nlohmann::json &json, FMaterialInfo::FCreationInfo &type);

            void to_json(nlohmann::json &json, const FMaterialInfo &type);
            void from_json(const nlohmann::json &json, FMaterialInfo &type);

            /**
             * @brief Load materials from file
             * 
             */
            class CMaterialLoader : public utl::singleton<CMaterialLoader>
            {
            public:
                CMaterialLoader();

                /**
                 * @brief Create material by name. All material create info stores in file "materials.json"
                 * 
                 * @param name Material name
                 * @return ref_ptr<Material::CMaterialBase> Smart pointer to material object
                 */
                ref_ptr<Material::CMaterialBase> create(const std::string &name);

            private:
                /**
                 * @brief Load file with material info
                 * 
                 */
                void load();

                /**
                 * @brief Save file with material info
                 * 
                 */
                void save();

                std::map<std::string, ref_ptr<Material::CMaterialBase>> mMaterialCache;
                FMaterialInfo data;
            };
        }
    }
}