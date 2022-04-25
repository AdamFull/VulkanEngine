#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/scene/objects/components/light/LightComponent.h"
#include "graphics/buffer/VulkanUniform.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class LightSourceManager : public utl::singleton<LightSourceManager>
            {
            public:
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(Resources::ELightSourceType eType, glm::vec3 position);
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color);
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius);
                std::shared_ptr<Scene::Objects::Components::Light::LightComponent> CreateSource(Resources::ELightSourceType eType, FTransform transform, glm::vec3 color, float attenuation_radius);

                std::vector<Core::FLight> GetSources();
            private:
                std::vector<std::shared_ptr<Scene::Objects::Components::Light::LightComponent>> m_vLights;
            };            
        }
    }
}