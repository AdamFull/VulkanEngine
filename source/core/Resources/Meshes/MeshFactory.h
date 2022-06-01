#pragma once
#include "Resources/ResourceCunstruct.h"

namespace engine
{
    namespace resources
    {
        namespace mesh
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
                 * @return ref_ptr<CMeshFragment> Smart pointer mesh object
                 */
                static ref_ptr<CMeshFragment> create(FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<ref_ptr<CMeshFragment>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}