#pragma once
#include "VulkanVertex.h"

namespace Engine
{
    namespace Types
    {
        namespace Mesh
        {
            struct Static
            {
                std::vector<FVertex> vertices;
                std::vector<uint32_t> indices;
            };
            
            void Load(Main::FVulkanEngine& engine, Static& mesh, std::string srMeshPath);
        }
    }
}