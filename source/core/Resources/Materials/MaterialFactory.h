#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    class MaterialBase;
    class ResourceManager;
    struct MaterialFactory
    {
        using material_t = std::unique_ptr<MaterialBase>;
    public:
        static std::shared_ptr<MaterialBase> Create(std::shared_ptr<ResourceManager> resourceMgr, FMaterialCreateInfo info);
    private:
        static std::map<EMaterialType, std::function<material_t()>> m_mFactory;
    };
}