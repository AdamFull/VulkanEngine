#pragma once
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        /**
         * @brief Simple plane primitive data
         * 
         */
        struct FPlane 
        {
            static std::vector<Core::FVertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };

        /**
         * @brief Simple cube primitive data
         * 
         */
        struct FCube 
        {
            static std::vector<Core::FVertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };

        /**
         * @brief Simple sphere primitive data
         * 
         */
        struct FSphere 
        {
            static std::vector<Core::FVertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };
    }
}