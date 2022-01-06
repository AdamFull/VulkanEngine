#pragma once
#include "ResourceCunstruct.h"

#include "Core/Image/Image.h"
#include "Materials/VulkanMaterial.h"
#include "Meshes/MeshFragment.h"
#include "Meshes/MeshFactory.h"
//Old code in this includer, remove
#include "Core/Image/TextureFactory.h"
#include "Materials/MaterialFactory.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager : public std::enable_shared_from_this<ResourceManager>
        {
        public:
            void Create();
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

            /*******************************For texture****************************/
            template <>
            void AddExisting<Core::Image>(std::string srResourceName, std::shared_ptr<Core::Image> pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Core::Image> Add(FTextureCreateInfo info)
            {
                std::shared_ptr<Core::Image> texture = Core::TextureFactory::Create(shared_from_this(), info);
                AddExisting(info.srName, texture);
                return nullptr;
            }

            template <>
            std::shared_ptr<Core::Image> Get(std::string srResourceName)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    return it->second;
                return m_mTextures.at("no_texture");
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
            std::shared_ptr<Material::MaterialBase> Add(FMaterialCreateInfo info)
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

            /*******************************For mesh****************************/
            template <>
            void AddExisting<Mesh::MeshFragment>(std::string srResourceName, std::shared_ptr<Mesh::MeshFragment> pResource)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    assert(false && "Resource named: is already exists.");
                m_mMeshes.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Mesh::MeshFragment> Add(FMeshCreateInfo info)
            {
                std::shared_ptr<Mesh::MeshFragment> mesh = Mesh::MeshFactory::Create(shared_from_this(), info);
                AddExisting(info.srName, mesh);
                return nullptr;
            }

            template <>
            std::shared_ptr<Mesh::MeshFragment> Get(std::string srResourceName)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    return it->second;
                return nullptr;
            }

        private:
            std::map<std::string, std::shared_ptr<Core::Image>> m_mTextures;
            std::map<std::string, std::shared_ptr<Material::MaterialBase>> m_mMaterials;
            std::map<std::string, std::shared_ptr<Mesh::MeshFragment>> m_mMeshes;
        };
    }
}