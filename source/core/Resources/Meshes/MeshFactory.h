#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    class MeshBase;
    class MaterialBase;
    class ResourceManager;
    struct MeshFactory
    {
        using mesh_t = std::shared_ptr<MeshBase>;
    public:
        static mesh_t Create(std::shared_ptr<ResourceManager> resourceMgr, FMeshCreateInfo info);
    private:
        static std::map<EMeshType, std::function<mesh_t(FMeshCreateInfo)>> m_mFactory;
    };
}