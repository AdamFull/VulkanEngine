#pragma once
#include "resources/ResourceCunstruct.h"
#include "MaterialLoader.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            /**
             * @brief Factory that creates material from create info
             * 
             */
            struct CMaterialFactory
            {
                using material_t = std::unique_ptr<CMaterialBase>;

            public:
                /**
                 * @brief Creating material uses name of material for load data from MaterialLoader
                 * 
                 * @param info Material create info
                 * @return std::shared_ptr<CMaterialBase> Material smart pointer object
                 */
                static std::shared_ptr<CMaterialBase> create(FMaterialCreateInfo info);
            };
        }
    }
}