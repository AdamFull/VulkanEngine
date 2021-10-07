#pragma once
#include "ResourceBase.h"
#include "Textures/VulkanTexture.h"
#include "Materials/VulkanMaterial.h"
#include "Meshes/VulkanMesh.h"

namespace Engine
{
    enum class EResourceType
    {
        eTexture,
        eMaterial,
        eMesh
    };

    enum class ETextureType
    {
        e2D,
        e3D,
        eCubemap
    };

    enum class ETextureAttachmentType
    {
        eColor,
        eMetalicRoughness,
        eNormal,
        eHeight,
        eOcclusion,
        eEmissive,
        eSpecular
    };

    struct FTextureCreateInfo
    {
        bool bInitialized{false};
        ETextureType eType;
        ETextureAttachmentType eAttachment;
        std::vector<unsigned char> vResourceData;
        uint32_t width, height, channels, mipLevels;
        vk::Format format;
    };

    enum class EMaterialType
    {
        eUI,
        eDiffuse,
        eGLTF
    };

    struct FMaterialCreateInfo
    {
        //Add material indexing
        bool bInitialized{false};
        EMaterialType eType;
        std::vector<FTextureCreateInfo> vTextures;
    };
 
    enum class EMeshType
    {
        eStatic,
        eSkeletal,
        eGLTF
    };

    struct FMeshCreateInfo
    {
        bool bInitialized{false};
        EMeshType eType;
        std::vector<FMaterialCreateInfo> vMaterials;
    };

    struct ResourceFactory
    {
        using texture_t = std::unique_ptr<TextureBase>;
        using material_t = std::unique_ptr<MaterialBase>;
        using mesh_t = std::unique_ptr<MeshBase>;

        template<class InfoType>
        static std::unique_ptr<ResourceBase> Create(InfoType createInfo)
        {
            return CreateResource(createInfo);
        }
    private:
        static texture_t CreateResource(FTextureCreateInfo info);
        static material_t CreateResource(FMaterialCreateInfo info);
        static mesh_t CreateResource(FMeshCreateInfo info);

        static std::map<ETextureType, std::function<texture_t()>> m_mTextureFactory;
        static std::map<EMaterialType, std::function<material_t()>> m_mMaterialFactory;
        static std::map<EMeshType, std::function<mesh_t()>> m_mMeshFactory;

    };
}