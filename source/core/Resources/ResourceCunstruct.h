#pragma once

namespace Engine
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
        eColor,
        eAlbedo,
        eMetalicRoughness,
        eNormal,
        eHeight,
        eOcclusion,
        eEmissive,
        eSpecular,
        eCubemap
    };

    struct FTextureCreateInfo
    {
        std::string srName;
        ETextureType eType;
        ETextureAttachmentType eAttachment;
        std::string srSrc;
    };

    enum class EMaterialType
    {
        eUI,
        eDiffuse,
        eSkybox
    };

    struct FMaterialParams
    {
        //glm::vec4 albedo{1.f};
        float alpha_cutoff{1.f};
        float metalic{1.f};
        float specular{1.f};
    };

    struct FMaterialCreateInfo
    {
        std::string srName;
        EMaterialType eType;
        std::vector<std::string> srAttachments;
        std::vector<FTextureCreateInfo> vTextures;
        FMaterialParams fParams;
    };
 
    enum class EMeshType
    {
        eStatic,
        eSkeletal,
        eGLTF
    };

    struct FMeshCreateInfo
    {
        std::string srName;
        EMeshType eType;
        std::string srSrc;
        std::vector<std::string> srAttachments;
        std::vector<FMaterialCreateInfo> vMaterials;
    };
}