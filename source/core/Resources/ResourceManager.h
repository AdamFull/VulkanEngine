#pragma once
#include "ResourceCunstruct.h"

#include "graphics/image/Image.h"
#include "graphics/image/Image2D.h"
#include "materials/VulkanMaterial.h"
//Old code in this includer, remove

namespace engine
{
    namespace resources
    {
        /**
         * @brief Resource manager just holding inside all created resources while engine works
         * 
         */
        class CResourceManager
        {
        public:
            ~CResourceManager();
            /**
             * @brief Creates resource manager with empty texture object
             * 
             */
            void create();

            /**
             * @brief Returns resource by name
             * 
             * @tparam ResType Resource type
             * @param srResourceName Resource name
             * @return utl::ref_ptr<ResType> Smart pointer to resource object
             */
            template <class ResType>
            utl::ref_ptr<ResType>& get(const std::string& srResourceName)
            {
                assert(false && "Cannot find resource type");
                return nullptr_ref(ResType);
            }

            /*******************************For texture****************************/
            /**
             * @brief Add existing texture object to resource manager
             */
            void addExisting(const std::string& srResourceName, utl::ref_ptr<core::CImage>& pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            /**
             * @brief Add existing texture2D object to resource manager
             */
            void addExisting(const std::string& srResourceName, utl::ref_ptr<core::CImage2D>& pResource)
            {
                auto it = m_mTextures.find(srResourceName);
                if (it != m_mTextures.end())
                    assert(false && "Resource named: is already exists.");
                m_mTextures.emplace(srResourceName, pResource);
            }

            /**
             * @brief Get smart pointer to created image
             * 
             * @param srResourceName Image name
             * @return utl::ref_ptr<Core::CImage> Smart pointer to resource object
             */
            template <>
            utl::ref_ptr<core::CImage>& get(const std::string& srResourceName)
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
            void addExisting(const std::string& srResourceName, utl::ref_ptr<material::CMaterialBase> pResource)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    assert(false && "Resource named: is already exists.");
                m_mMaterials.emplace(srResourceName, pResource);
            }

            /**
             * @brief Get smart pointer to created material
             * 
             * @param srResourceName Material name
             * @return utl::ref_ptr<Material::CMaterialBase> Smart pointer to resource object
             */
            template <>
            utl::ref_ptr<material::CMaterialBase>& get(const std::string& srResourceName)
            {
                auto it = m_mMaterials.find(srResourceName);
                if (it != m_mMaterials.end())
                    return it->second;
                return pNullMat;
            }

        private:
            void cleanup();
            std::map<std::string, utl::ref_ptr<core::CImage>> m_mTextures;
            std::map<std::string, utl::ref_ptr<material::CMaterialBase>> m_mMaterials;

            utl::ref_ptr<core::CImage> pNullImage{ nullptr };
            utl::ref_ptr<core::CImage2D> pNullImage2D{ nullptr };
            utl::ref_ptr<material::CMaterialBase> pNullMat{ nullptr };
        };
    }
}