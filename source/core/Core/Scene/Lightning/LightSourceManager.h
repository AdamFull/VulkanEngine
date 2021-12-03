#pragma once
#include "Core/VulkanUniform.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                namespace Components
                {
                    namespace Light { class LightComponent; }
                }
            }
            enum class ELightSourceType
            {
                ePoint
            };

            class LightSourceManager : public Singleton<LightSourceManager>
            {
            public:
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(ELightSourceType eType, glm::vec3 position);
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(ELightSourceType eType, glm::vec3 position, glm::vec3 color);
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius);

                std::vector<Core::FLight> GetSources();
            private:
                std::vector<std::shared_ptr<Scene::Objects::Components::Light::LightComponent>> m_vLights;
            };            
        }
    }
}