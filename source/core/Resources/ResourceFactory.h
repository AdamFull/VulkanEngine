#pragma once
#include "ResourceBase.h"
#include "Textures/VulkanTexture.h"
#include "Materials/VulkanMaterial.h"
#include "Meshes/VulkanMesh.h"

namespace Engine
{
    class ResourceManager;

    struct ResourceFactory
    {
        using texture_t = std::unique_ptr<TextureBase>;
        using material_t = std::unique_ptr<MaterialBase>;
        using mesh_t = std::unique_ptr<MeshBase>;

        static texture_t CreateTexture(std::shared_ptr<ResourceManager> resourceMgr, FTextureCreateInfo info);
        static material_t CreateMaterial(std::shared_ptr<ResourceManager> resourceMgr, FMaterialCreateInfo info);
        static mesh_t CreateMesh(std::shared_ptr<ResourceManager> resourceMgr, FMeshCreateInfo info);
    private:
        static std::map<ETextureType, std::function<texture_t(FTextureCreateInfo)>> m_mTextureFactory;
        static std::map<EMaterialType, std::function<material_t()>> m_mMaterialFactory;
        static std::map<EMeshType, std::function<mesh_t(FMeshCreateInfo)>> m_mMeshFactory;

    };
}