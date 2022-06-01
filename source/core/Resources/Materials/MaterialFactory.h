#pragma once
#include "resources/ResourceCunstruct.h"
#include "MaterialLoader.h"

namespace engine
{
    namespace resources
    {
        namespace material
        {
            /**
             * @brief Factory that creates material from create info
             * 
             */
            struct CMaterialFactory
            {
                using material_t = scope_ptr<CMaterialBase>;

            public:
                /**
                 * @brief Creating material uses name of material for load data from MaterialLoader
                 * 
                 * @param info Material create info
                 * @return ref_ptr<CMaterialBase> Material smart pointer object
                 */
                static ref_ptr<CMaterialBase> create(FMaterialCreateInfo info);
            };
        }
    }
}