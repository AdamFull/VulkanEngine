#pragma once
#include "ResourceCunstruct.h"

#include "Textures/VulkanTexture.h"
#include "Materials/VulkanMaterial.h"
#include "Meshes/VulkanMesh.h"

namespace Engine
{
    class ResourceManager : public std::enable_shared_from_this<ResourceManager>
    {
    public:
        void Load(std::string srResourcesPath);

        template<class ResType>
        void AddExisting(std::string srResourceName, std::shared_ptr<ResType> pResource)
        {
            assert(false && "Cannot find resource type");
        }

        template<class ResType>
        std::shared_ptr<ResType> Get(std::string srResourceName)
        {
            assert(false && "Cannot find resource type");
            return nullptr;
        }

        template<class ResType>
        void Destroy(std::string srResourceName)
        {
            assert(false && "Cannot find resource type");
        }

        /*******************************For texture****************************/
        template<>
        void AddExisting<TextureBase>(std::string srResourceName, std::shared_ptr<TextureBase> pResource)
        {
            auto it = m_mTextures.find(srResourceName);
            if(it != m_mTextures.end())
                assert(false && "Resource named: is already exists.");
            m_mTextures.emplace(srResourceName, pResource);
        }

        template<>
        std::shared_ptr<TextureBase> Get(std::string srResourceName)
        {
            auto it = m_mTextures.find(srResourceName);
            if(it != m_mTextures.end())
                return it->second;
            return nullptr;
        }

        template<>
        void Destroy<TextureBase>(std::string srResourceName)
        {
            auto it = m_mTextures.find(srResourceName);
            if(it != m_mTextures.end())
                it->second->Destroy();
            assert(false && "Cannot find resource named: .");
        }

        /*******************************For material****************************/
        template<>
        void AddExisting<MaterialBase>(std::string srResourceName, std::shared_ptr<MaterialBase> pResource)
        {
            auto it = m_mMaterials.find(srResourceName);
            if(it != m_mMaterials.end())
                assert(false && "Resource named: is already exists.");
            m_mMaterials.emplace(srResourceName, pResource);
        }

        template<>
        std::shared_ptr<MaterialBase> Get(std::string srResourceName)
        {
            auto it = m_mMaterials.find(srResourceName);
            if(it != m_mMaterials.end())
                return it->second;
            return nullptr;
        }

        template<>
        void Destroy<MaterialBase>(std::string srResourceName)
        {
            auto it = m_mMaterials.find(srResourceName);
            if(it != m_mMaterials.end())
                it->second->Destroy();
            assert(false && "Cannot find resource named: .");
        }

        /*******************************For mesh****************************/
        template<>
        void AddExisting<MeshBase>(std::string srResourceName, std::shared_ptr<MeshBase> pResource)
        {
            auto it = m_mMeshes.find(srResourceName);
            if(it != m_mMeshes.end())
                assert(false && "Resource named: is already exists.");
            m_mMeshes.emplace(srResourceName, pResource);
        }

        template<>
        std::shared_ptr<MeshBase> Get(std::string srResourceName)
        {
            auto it = m_mMeshes.find(srResourceName);
            if(it != m_mMeshes.end())
                return it->second;
            return nullptr;
        }

        template<>
        void Destroy<MeshBase>(std::string srResourceName)
        {
            auto it = m_mMeshes.find(srResourceName);
            if(it != m_mMeshes.end())
                it->second->Destroy();
            assert(false && "Cannot find resource named: .");
        }

        void DestroyAll();
    private:
        std::map<std::string, std::shared_ptr<TextureBase>> m_mTextures;
        std::map<std::string, std::shared_ptr<MaterialBase>> m_mMaterials;
        std::map<std::string, std::shared_ptr<MeshBase>> m_mMeshes;
    };
}