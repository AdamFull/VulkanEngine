#pragma once
#include "ResourceCunstruct.h"

#include "graphics/image/Image.h"
#include "graphics/image/Image2D.h"
#include "materials/VulkanMaterial.h"
#include "meshes/MeshFragment.h"
#include "meshes/MeshFactory.h"
#include "materials/MaterialFactory.h"
//Old code in this includer, remove

namespace engine
{
    namespace resources
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
             * @return ref_ptr<ResType> Smart pointer to resource object
             */
            template <class ResType, class InfoType>
            ref_ptr<ResType>& add(const InfoType& info)
            {
                assert(false && "Cannot find resource type");
                return nullptr;
            }

            /**
             * @brief Returns resource by name
             * 
             * @tparam ResType Resource type
             * @param srResourceName Resource name
             * @return ref_ptr<ResType> Smart pointer to resource object
             */
            template <class ResType>
            ref_ptr<ResType>& get(const std::string& srResourceName)
            {
                assert(false && "Cannot find resource type");
                return nullptr_ref(ResType);
            }

            /*******************************For texture****************************/
            /**
             * @brief Add existing texture object to resource manager
             */
            void addExisting(const std::string& srResourceName, ref_ptr<core::CImage>& pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            /**
             * @brief Add existing texture2D object to resource manager
             */
            void addExisting(const std::string& srResourceName, ref_ptr<core::CImage2D>& pResource)
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
             * @return ref_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            ref_ptr<core::CImage>& add(const FTextureCreateInfo& info)
            {
                ref_ptr<core::CImage> texture = make_scope<core::CImage>();
                texture->create(info.srSrc);
                addExisting(info.srName, texture);
                return pNullImage;
            }

            /**
             * @brief Specialization for image2D object (texture)
             * 
             * @param info Image create info 
             * @return ref_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            ref_ptr<core::CImage2D>& add(const FTextureCreateInfo& info)
            {
                ref_ptr<core::CImage> texture = make_scope<core::CImage2D>();
                texture->create(info.srSrc);
                addExisting(info.srName, texture);
                return pNullImage2D;
            }

            /**
             * @brief Get smart pointer to created image
             * 
             * @param srResourceName Image name
             * @return ref_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            ref_ptr<core::CImage>& get(const std::string& srResourceName)
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
            void addExisting(const std::string& srResourceName, ref_ptr<material::CMaterialBase> pResource)
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
             * @return ref_ptr<Material::CMaterialBase> Smart pointer to resource object
             */
            template <>
            ref_ptr<material::CMaterialBase>& add(const FMaterialCreateInfo& info)
            {
                ref_ptr<material::CMaterialBase> material = material::CMaterialFactory::create(info);
                addExisting(info.srName, material);
                return pNullMat;
            }

            /**
             * @brief Get smart pointer to created material
             * 
             * @param srResourceName Material name
             * @return ref_ptr<Material::CMaterialBase> Smart pointer to resource object
             */
            template <>
            ref_ptr<material::CMaterialBase>& get(const std::string& srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    return it->second;
                return pNullMat;
            }

            /*******************************For mesh****************************/
            /**
             * @brief Add existing Mesh object to resource manager
             */
            void addExisting(const std::string& srResourceName, ref_ptr<mesh::CMeshFragment> pResource)
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
             * @return ref_ptr<Mesh::CMeshFragment> Smart pointer to resource object
             */
            template <>
            ref_ptr<mesh::CMeshFragment>& add(const FMeshCreateInfo& info)
            {
                ref_ptr<mesh::CMeshFragment> mesh = mesh::CMeshFactory::create(info);
                addExisting(info.srName, mesh);
                return pNullFrag;
            }

            /**
             * @brief Get smart pointer to created mesh
             * 
             * @param srResourceName Mesh name
             * @return ref_ptr<Mesh::CMeshFragment> Smart pointer to resource object
             */
            template <>
            ref_ptr<mesh::CMeshFragment>& get(const std::string& srResourceName)
            {
                auto it = m_mMeshes.find(srResourceName);
                if (it != m_mMeshes.end())
                    return it->second;
                return pNullFrag;
            }

        private:
            std::map<std::string, ref_ptr<core::CImage>> m_mTextures;
            std::map<std::string, ref_ptr<material::CMaterialBase>> m_mMaterials;
            std::map<std::string, ref_ptr<mesh::CMeshFragment>> m_mMeshes;

            ref_ptr<core::CImage> pNullImage{ nullptr };
            ref_ptr<core::CImage2D> pNullImage2D{ nullptr };
            ref_ptr<material::CMaterialBase> pNullMat{ nullptr };
            ref_ptr<mesh::CMeshFragment> pNullFrag{ nullptr };
        };
    }
}