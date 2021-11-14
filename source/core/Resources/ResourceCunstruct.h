#pragma once

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
            eDiffuseAlbedo,
            eMetalicRoughness,
            eSpecularGlossiness,
            eEmissive,
            eNormal,
            eHeight,
            eOcclusion,
            //Deferred
            ePosition,
            eDepth,
            //Special
            eCubemap,
            eIrradiance,
            eBRDFLUT,
            ePrefiltred
        };

        enum class ENoisePattern
        {
            ePerlin,
            eFractal
        };

        struct FNoiseParam
        {
            ENoisePattern pattern{};
            uint32_t width{0};
            uint32_t height{0};
            uint32_t depth{0};
        };

        struct FTextureCreateInfo
        {
            std::string srName{};
            ETextureType eType{};
            ETextureAttachmentType eAttachment{};
            std::string srSrc{};
            FNoiseParam noise{};
        };

        enum class EMaterialType
        {
            eUI,
            eDiffuse,
            eSkybox,
            ePBR
        };

        struct FMaterialParamsInfo
        {
            // glm::vec4 albedo{1.f};
            float alpha_cutoff{1.f};
            float metalic{1.f};
            float specular{1.f};
        };

        struct FMaterialCreateInfo
        {
            std::string srName{};
            std::string srPrimitive{};
            EMaterialType eType{};
            std::vector<std::string> srAttachments{};
            std::vector<FTextureCreateInfo> vTextures{};
            FMaterialParamsInfo fParams{};
        };

        enum class EMeshType
        {
            eStatic,
            eSkeletal,
            eGLTF
        };

        struct FMeshCreateInfo
        {
            std::string srName{};
            EMeshType eType{};
            std::string srSrc{};
            bool bUseIncludedMaterial{false};
            std::vector<FMaterialCreateInfo> vMaterials{};
        };
    }
}