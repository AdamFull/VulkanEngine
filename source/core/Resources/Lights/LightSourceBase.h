#pragma once

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Light
        {
            class LightSourceBase
            {
            public:
                void Create(std::shared_ptr<ResourceManager> pResMgr);
                void Update(uint32_t imageIndex);

                glm::vec3 position{};
                glm::vec3 color{};
                float attenuation{};
            };
        }
    }
}