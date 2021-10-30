#pragma once
#include "ResourceCunstruct.h"

#include "Textures/VulkanTexture.h"
#include "Materials/VulkanMaterial.h"
#include "Meshes/VulkanMesh.h"

#include "Textures/TextureFactory.h"
#include "Materials/MaterialFactory.h"
#include "Meshes/MeshFactory.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager : public std::enable_shared_from_this<ResourceManager>
        {
        public:
            void Load(std::string srResourcesPath);

            template <class ResType>
            void AddExisting(std::string srResourceName, std::shared_ptr<ResType> pResource)
            {
                assert(false && "Cannot find resource type");
            }

            template <class ResType, class InfoType>
            std::shared_ptr<ResType> Add(InfoType info)
            {
                assert(false && "Cannot find resource type");
                return nullptr;
            }

            template <class ResType>
            std::shared_ptr<ResType> Get(std::string srResourceName)
            {
                assert(false && "Cannot find resource type");
                return nullptr;
            }

            template <class ResType>
            void Destroy(std::string srResourceName)
            {
                assert(false && "Cannot find resource type");
            }

            /*******************************For texture****************************/
            template <>
            void AddExisting<Texture::TextureBase>(std::string srResourceName, std::shared_ptr<Texture::TextureBase> pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Texture::TextureBase> ResourceManager::Add(FTextureCreateInfo info)
            {
                std::shared_ptr<Texture::TextureBase> texture = Texture::TextureFactory::Create(shared_from_this(), info);
                AddExisting(info.srName, texture);
                return nullptr;
            }

            template <>
            std::shared_ptr<Texture::TextureBase> Get(std::string srResourceName)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    return it->second;
                return nullptr;
            }

            template <>
            void Destroy<Texture::TextureBase>(std::string srResourceName)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    it->second->Destroy();
                assert(false && "Cannot find resource named: .");
            }

            /*******************************For material****************************/
            template <>
            void AddExisting<Material::MaterialBase>(std::string srResourceName, std::shared_ptr<Material::MaterialBase> pResource)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    assert(false && "Resource named: is already exists.");
                m_mMaterials.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Material::MaterialBase> ResourceManager::Add(FMaterialCreateInfo info)
            {
                std::shared_ptr<Material::MaterialBase> material = Material::MaterialFactory::Create(shared_from_this(), info);
                AddExisting(info.srName, material);
                return nullptr;
            }

            template <>
            std::shared_ptr<Material::MaterialBase> Get(std::string srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    return it->second;
                return nullptr;
            }

            template <>
            void Destroy<Material::MaterialBase>(std::string srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    it->second->Destroy();
                assert(false && "Cannot find resource named: .");
            }

            /*******************************For mesh****************************/
            template <>
            void AddExisting<Mesh::MeshBase>(std::string srResourceName, std::shared_ptr<Mesh::MeshBase> pResource)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    assert(false && "Resource named: is already exists.");
                m_mMeshes.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Mesh::MeshBase> ResourceManager::Add(FMeshCreateInfo info)
            {
                std::shared_ptr<Mesh::MeshBase> mesh = Mesh::MeshFactory::Create(shared_from_this(), info);
                AddExisting(info.srName, mesh);
                return nullptr;
            }

            template <>
            std::shared_ptr<Mesh::MeshBase> Get(std::string srResourceName)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    return it->second;
                return nullptr;
            }

            template <>
            void Destroy<Mesh::MeshBase>(std::string srResourceName)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    it->second->Destroy();
                assert(false && "Cannot find resource named: .");
            }

            void DestroyAll();

        private:
            std::map<std::string, std::shared_ptr<Texture::TextureBase>> m_mTextures;
            std::map<std::string, std::shared_ptr<Material::MaterialBase>> m_mMaterials;
            std::map<std::string, std::shared_ptr<Mesh::MeshBase>> m_mMeshes;
        };
    }
}