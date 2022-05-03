#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class CResourceManager;
        namespace Mesh
        {
            class CMeshFragment;
            struct CMeshFactory
            {
            public:
                static std::shared_ptr<CMeshFragment> create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<std::shared_ptr<CMeshFragment>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}