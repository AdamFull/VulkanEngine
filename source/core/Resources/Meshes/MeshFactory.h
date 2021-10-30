#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Mesh
        {
            class MeshBase;

            struct MeshFactory
            {
            public:
                static std::shared_ptr<MeshBase> Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMeshCreateInfo info);

            private:
                static std::map<EMeshType, std::function<std::shared_ptr<MeshBase>(FMeshCreateInfo)>> m_mFactory;
            };
        }
    }

}