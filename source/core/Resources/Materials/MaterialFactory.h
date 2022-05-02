#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        class CResourceManager;
        namespace Material
        {
            class CMaterialBase;

            struct CMaterialFactory
            {
                using material_t = std::unique_ptr<CMaterialBase>;

            public:
                static std::shared_ptr<CMaterialBase> create(std::shared_ptr<Resources::CResourceManager> resourceMgr, FMaterialCreateInfo info);

            private:
                static std::map<EMaterialType, std::function<material_t()>> m_mFactory;
            };
        }
    }
}