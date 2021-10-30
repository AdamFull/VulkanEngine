#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Material
        {
            class MaterialBase;

            struct MaterialFactory
            {
                using material_t = std::unique_ptr<MaterialBase>;

            public:
                static std::shared_ptr<MaterialBase> Create(std::shared_ptr<Resources::ResourceManager> resourceMgr, FMaterialCreateInfo info);

            private:
                static std::map<EMaterialType, std::function<material_t()>> m_mFactory;
            };
        }
    }
}