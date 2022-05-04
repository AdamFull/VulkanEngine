#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            class CMeshFragment;
            struct CMeshFactory
            {
            public:
                static std::shared_ptr<CMeshFragment> create(FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<std::shared_ptr<CMeshFragment>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}