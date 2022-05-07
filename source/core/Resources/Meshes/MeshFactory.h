#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class CMeshFragment;
            /**
             * @brief Mesh creation factory
             * 
             */
            struct CMeshFactory
            {
            public:
                /**
                 * @brief Creates mesh objetc from info
                 * 
                 * @param info Creation info
                 * @return std::shared_ptr<CMeshFragment> Smart pointer mesh object
                 */
                static std::shared_ptr<CMeshFragment> create(FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<std::shared_ptr<CMeshFragment>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}