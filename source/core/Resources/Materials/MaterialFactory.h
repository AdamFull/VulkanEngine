#pragma once
#include "resources/ResourceCunstruct.h"
#include "MaterialLoader.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            struct CMaterialFactory
            {
                using material_t = std::unique_ptr<CMaterialBase>;

            public:
                static std::shared_ptr<CMaterialBase> create(FMaterialCreateInfo info);
            };
        }
    }
}