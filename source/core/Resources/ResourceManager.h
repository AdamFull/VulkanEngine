#pragma once
#include "ResourceCunstruct.h"

#include "graphics/image/Image.h"
#include "graphics/image/Image2D.h"
#include "materials/VulkanMaterial.h"
#include "meshes/MeshFragment.h"
#include "meshes/MeshFactory.h"
#include "materials/MaterialFactory.h"
//Old code in this includer, remove

namespace Engine
{
    namespace Resources
    {
        /**
         * @brief Resource manager just holding inside all created resources while engine works
         * 
         */
        class CResourceManager : public utl::singleton<CResourceManager>
        {
        public:
            /**
             * @brief Creates resource manager with empty texture object
             * 
             */
            void create();

            /**
             * @brief Loads resource manager from file
             * 
             * @param srResourcesPath Path to resources file
             */
            void load(std::string srResourcesPath);

            /**
             * @brief Add resource template function
             * 
             * @tparam ResType Resource type
             * @tparam InfoType Type of resource info
             * @param info Resource info object
             * @return std::shared_ptr<ResType> Smart pointer to resource object
             */
            template <class ResType, class InfoType>
            std::shared_ptr<ResType> add(const InfoType& info)
            {
                assert(false && "Cannot find resource type");
                return nullptr;
            }

            /**
             * @brief Returns resource by name
             * 
             * @tparam ResType Resource type
             * @param srResourceName Resource name
             * @return std::shared_ptr<ResType> Smart pointer to resource object
             */
            template <class ResType>
            std::shared_ptr<ResType> get(const std::string& srResourceName)
            {
                assert(false && "Cannot find resource type");
                return nullptr;
            }

            /*******************************For texture****************************/
            /**
             * @brief Add existing texture object to resource manager
             */
            void addExisting(const std::string& srResourceName, std::shared_ptr<Core::CImage> pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            /**
             * @brief Add existing texture2D object to resource manager
             */
            void addExisting(const std::string& srResourceName, std::shared_ptr<Core::CImage2D> pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            /**
             * @brief Specialization for image object (texture)
             * 
             * @param info Image create info 
             * @return std::shared_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Core::CImage> add(const FTextureCreateInfo& info)
            {
                std::shared_ptr<Core::CImage> texture = std::make_unique<Core::CImage>();
                texture->create(info.srSrc);
                addExisting(info.srName, texture);
                return nullptr;
            }

            /**
             * @brief Specialization for image2D object (texture)
             * 
             * @param info Image create info 
             * @return std::shared_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Core::CImage2D> add(const FTextureCreateInfo& info)
            {
                std::shared_ptr<Core::CImage> texture = std::make_unique<Core::CImage2D>();
                texture->create(info.srSrc);
                addExisting(info.srName, texture);
                return nullptr;
            }

            /**
             * @brief Get smart pointer to created image
             * 
             * @param srResourceName Image name
             * @return std::shared_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Core::CImage> get(const std::string& srResourceName)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    return it->second;
                return m_mTextures.at("no_texture");
            }

            /*******************************For material****************************/
            /**
             * @brief Add existing Material object to resource manager
             */
            void addExisting(const std::string& srResourceName, std::shared_ptr<Material::CMaterialBase> pResource)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    assert(false && "Resource named: is already exists.");
                m_mMaterials.emplace(srResourceName, pResource);
            }

            /**
             * @brief Specialization for MaterialBase object (texture)
             * 
             * @param info Material creation info
             * @return std::shared_ptr<Material::CMaterialBase> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Material::CMaterialBase> add(const FMaterialCreateInfo& info)
            {
                std::shared_ptr<Material::CMaterialBase> material = Material::CMaterialFactory::create(info);
                addExisting(info.srName, material);
                return nullptr;
            }

            /**
             * @brief Get smart pointer to created material
             * 
             * @param srResourceName Material name
             * @return std::shared_ptr<Material::CMaterialBase> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Material::CMaterialBase> get(const std::string& srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    return it->second;
                return nullptr;
            }

            /*******************************For mesh****************************/
            /**
             * @brief Add existing Mesh object to resource manager
             */
            void addExisting(const std::string& srResourceName, std::shared_ptr<Mesh::CMeshFragment> pResource)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    assert(false && "Resource named: is already exists.");
                m_mMeshes.emplace(srResourceName, pResource);
            }

            /**
             * @brief Specialization for Mesh object (texture)
             * 
             * @param info Mesh creation info
             * @return std::shared_ptr<Mesh::CMeshFragment> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Mesh::CMeshFragment> add(const FMeshCreateInfo& info)
            {
                std::shared_ptr<Mesh::CMeshFragment> mesh = Mesh::CMeshFactory::create(info);
                addExisting(info.srName, mesh);
                return nullptr;
            }

            /**
             * @brief Get smart pointer to created mesh
             * 
             * @param srResourceName Mesh name
             * @return std::shared_ptr<Mesh::CMeshFragment> Smart pointer to resource object
             */
            template <>
            std::shared_ptr<Mesh::CMeshFragment> get(const std::string& srResourceName)
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