#pragma once
#include "resources/ResourceCunstruct.h"
#include "MaterialLoader.h"

namespace Engine
{
    namespace Resources
    {
        class CResourceManager;
        namespace Material
        {
            struct CMaterialFactory
            {
                using material_t = std::unique_ptr<CMaterialBase>;

            public:
                static std::shared_ptr<CMaterialBase> create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMaterialCreateInfo info);
            };
        }
    }
}