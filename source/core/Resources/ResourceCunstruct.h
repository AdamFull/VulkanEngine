#pragma once
#include <glm/glm.hpp>
#include "serializer/Serialization.hpp"

namespace Engine
{
    namespace Resources
    {
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
        struct FTextureCreateInfo
        {
            /// @brief Texture name
            std::string srName{};
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
        struct FMaterialCreateInfo
        {
            /// @brief Material name. This parameter searches material in CMaterialLoader class
            std::string srName{};
            /// @brief Additional textures for material
            std::vector<FTextureCreateInfo> vTextures{};
            /// @brief Additional material params
            FMaterialParamsInfo fParams{};
        };

        enum class EMeshType
        {
            eStatic,
            eSkeletal,
            eGLTF
        };

        /**
         * @brief Mesh creation info
         * 
         */
        struct FMeshCreateInfo
        {
            /// @brief Mesh name
            std::string srName{};
            /// @brief Mesh type
            EMeshType eType{};
            /// @brief Mesh source path
            std::string srSrc{};
            /// @brief Texture repeat value
            float fRepeat{1.f};
            /// @brief Use custom attached material or use included
            bool bUseIncludedMaterial{false};
            /// @brief Custom attached materials
            std::vector<FMaterialCreateInfo> vMaterials{};
        };

        enum class ELightSourceType
        {
            ePoint,
            eSpot,
            eDirectional
        };

        /**
         * @brief Light source creation info
         * 
         */
        struct FLightCreateinfo
        {
            /// @brief Light source name
            std::string srName{};
            /// @brief Light source type
            ELightSourceType eType{};
            /// @brief Light color
            glm::vec4 vColor{};
            /// @brief  Light attenuation radius
            float fAttenuation{};
            /// @brief  Light intencity
            float fIntencity{1.f};
        };

        REGISTER_SERIALIZATION_BLOCK_H(FNoiseParam);
        REGISTER_SERIALIZATION_BLOCK_H(FTextureCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMaterialParamsInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMaterialCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FMeshCreateInfo);
        REGISTER_SERIALIZATION_BLOCK_H(FLightCreateinfo);
    }
}