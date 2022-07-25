#pragma once
#include "serializer/Serialization.hpp"

namespace engine
{
    namespace resources
    {
        struct FCreateInfoBase
        {
            /// @brief Light source name
            std::string srName{};
        };

        enum class ETextureType
        {
            e2D,
            e3D,
            eCubemap,
            eArray
        };

        enum class ETextureAttachmentType
        {
            eEmpty,
            eBRDFLUT,
            eIrradiance,
            ePrefiltred,
            
            ePosition,
            eLightningMask,
            eNormal,
            eDiffuseAlbedo,
            eMetalicRoughness,
            eSpecularGlossiness,
            eEmissive,
            eHeight,
            eOcclusion,
            eMRAH,
            eBrightness,
            eDepth,

            eCubemap
        };

        enum class ENoisePattern
        {
            ePerlin,
            eFractal
        };

        /**
         * @brief Information for 3D texture generation
         * 
         */
        struct FNoiseParam
        {
            /// @brief Noise pattern type
            ENoisePattern pattern{};
            /// @brief Noise width
            uint32_t width{0};
            /// @brief Noise height
            uint32_t height{0};
            /// @brief Noise depth
            uint32_t depth{0};
        };

        /**
         * @brief Texture creation info
         * 
         */
        struct FTextureCreateInfo : FCreateInfoBase
        {
            /// @brief Texture type
            ETextureType eType{};
            /// @brief Texture attachment
            std::string attachment{};
            /// @brief Texture source path
            std::string srSrc{};
            /// @brief Noise parameters. Uses when type is e3D
            FNoiseParam noise{};
        };

        /**
         * @brief Additional material parameters
         * 
         */
        struct FMaterialParamsInfo
        {
            /// @brief Alpha cutoff parameter
            float alpha_cutoff{1.f};
            /// @brief Metallness parameter
            float metalic{1.f};
            /// @brief Specular parameter
            float specular{1.f};
        };

        /**
         * @brief Material create info
         * 
         */
        struct FMaterialCreateInfo : FCreateInfoBase
        {
            /// @brief Additional textures for material
            std::vector<FTextureCreateInfo> vTextures{};
            /// @brief Additional material params
            FMaterialParamsInfo fParams{};
        };

        struct FSkyboxCreateInfo : FCreateInfoBase
        {
            FTextureCreateInfo environment;
        };

        /**
         * @brief Mesh creation info
         * 
         */
        struct FMeshCreateInfo : FCreateInfoBase
        {
            /// @brief Mesh source path
            std::string srSrc{};
            /// @brief Texture repeat value
            float fRepeat{1.f};
        };

        enum class ELightSourceType
        {
            ePoint,
            eDirectional,
            eSpot
        };

        /**
         * @brief Light source creation info
         * 
         */
        struct FLightCreateinfo : FCreateInfoBase
        {
            /// @brief Light source type
            ELightSourceType eType{};
            /// @brief Light color
            glm::vec4 vColor{};
            /// @brief  Light intencity
            float fIntencity{1.f};
            /// @brief  Light radius
            float fRadius{1.f};
            /// @brief  Light inner angle
            float fInnerAngle{0.f};
            /// @brief  Light outer agle
            float fOuterAngle{0.78f};
        };

        struct FCameraCreateInfo : FCreateInfoBase
        {
            float fFov{45.0f};
            float fNear{0.1f};
            float fFar{64.0f};
        };

        struct FStriptsCreateInfo : FCreateInfoBase
        {
            std::vector<std::string> scripts;
        };

        REGISTER_SERIALIZATION_BLOCK_H(FNoiseParam);
        REGISTER_SERIALIZATION_BLOCK_H(FTextureCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMaterialParamsInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMaterialCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FSkyboxCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMeshCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FLightCreateinfo);
        REGISTER_SERIALIZATION_BLOCK_H(FCameraCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FStriptsCreateInfo);
    }
}