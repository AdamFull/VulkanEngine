#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Mesh
        {
            class MeshFragment;

            struct MeshFactory
            {
            public:
                static std::shared_ptr<MeshFragment> Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<std::shared_ptr<MeshFragment>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}