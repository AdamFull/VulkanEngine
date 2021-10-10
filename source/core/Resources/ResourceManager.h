#pragma once
#include "ResourceFactory.h"

namespace Engine
{
    enum class EResourceType
    {
        eTexture,
        eMaterial,
        eMesh
    };

    class ResourceManager
    {
    protected:
        ResourceManager() = default;
        static std::unique_ptr<ResourceManager> m_pInstance;
    public:
        ResourceManager(const ResourceManager&) = delete;
        void operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        static std::unique_ptr<ResourceManager>& GetInstance();

        void Load(std::string srResourcesPath);

        void AddResource(EResourceType eResType, std::string srResourceName, std::shared_ptr<ResourceBase> pResource);

        std::shared_ptr<ResourceBase> GetResource(EResourceType eResType, std::string srResourceName);

        void Destroy(std::string srResourceName);
        void DestroyAll();
    private:
        std::map<std::string, std::shared_ptr<TextureBase>> m_mTextures;
        std::map<std::string, std::shared_ptr<MaterialBase>> m_mMaterials;
        std::map<std::string, std::shared_ptr<MeshBase>> m_mMeshes;
    };
}