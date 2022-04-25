#pragma once
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        struct FPlane 
        {
            static std::vector<Core::Vertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };

        struct FCube 
        {
            static std::vector<Core::Vertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };

        struct FSphere 
        {
            static std::vector<Core::Vertex> m_vVertices;
            static std::vector<uint32_t> m_vIndices;
        };
    }
}