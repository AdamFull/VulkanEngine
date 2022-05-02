#pragma once
#include "ResourceCunstruct.h"

#include "graphics/image/Image.h"
#include "materials/VulkanMaterial.h"
#include "meshes/MeshFragment.h"
#include "meshes/MeshFactory.h"
//Old code in this includer, remove
#include "materials/MaterialFactory.h"

namespace Engine
{
    namespace Resources
    {
        class CResourceManager : public std::enable_shared_from_this<CResourceManager>
        {
        public:
            void create();
            void load(std::string srResourcesPath);

            template <class ResType>
            void addExisting(std::string srResourceName, std::shared_ptr<ResType> pResource)
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
            void addExisting<Core::CImage>(std::string srResourceName, std::shared_ptr<Core::CImage> pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Core::CImage> Add(FTextureCreateInfo info)
            {
                std::shared_ptr<Core::CImage> texture = std::make_unique<Core::CImage>();
                texture->loadFromFile(info.srSrc);
                addExisting(info.srName, texture);
                return nullptr;
            }

            template <>
            std::shared_ptr<Core::CImage> Get(std::string srResourceName)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    return it->second;
                return m_mTextures.at("no_texture");
            }

            /*******************************For material****************************/
            template <>
            void addExisting<Material::CMaterialBase>(std::string srResourceName, std::shared_ptr<Material::CMaterialBase> pResource)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    assert(false && "Resource named: is already exists.");
                m_mMaterials.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Material::CMaterialBase> Add(FMaterialCreateInfo info)
            {
                std::shared_ptr<Material::CMaterialBase> material = Material::CMaterialFactory::create(shared_from_this(), info);
                addExisting(info.srName, material);
                return nullptr;
            }

            template <>
            std::shared_ptr<Material::CMaterialBase> Get(std::string srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    return it->second;
                return nullptr;
            }

            /*******************************For mesh****************************/
            template <>
            void addExisting<Mesh::CMeshFragment>(std::string srResourceName, std::shared_ptr<Mesh::CMeshFragment> pResource)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    assert(false && "Resource named: is already exists.");
                m_mMeshes.emplace(srResourceName, pResource);
            }

            template <>
            std::shared_ptr<Mesh::CMeshFragment> Add(FMeshCreateInfo info)
            {
                std::shared_ptr<Mesh::CMeshFragment> mesh = Mesh::CMeshFactory::create(shared_from_this(), info);
                addExisting(info.srName, mesh);
                return nullptr;
            }

            template <>
            std::shared_ptr<Mesh::CMeshFragment> Get(std::string srResourceName)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    return it->second;
                return nullptr;
            }

        private:
            std::map<std::string, std::shared_ptr<Core::CImage>> m_mTextures;
            std::map<std::string, std::shared_ptr<Material::CMaterialBase>> m_mMaterials;
            std::map<std::string, std::shared_ptr<Mesh::CMeshFragment>> m_mMeshes;
        };
    }
}