#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Light
        {
            class LightSourceBase : public ResourceBase
            {
            public:
                void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                void Update(uint32_t imageIndex) override;

                glm::vec4 position{};
                glm::vec3 color{};
                float attenuation{};
            };
        }
    }
}