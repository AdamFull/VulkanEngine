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
            class CLightSourceManager : public utl::singleton<CLightSourceManager>
            {
            public:
                std::shared_ptr<Scene::CLightComponent> createSource(Resources::ELightSourceType eType, glm::vec3 position);
                std::shared_ptr<Scene::CLightComponent> createSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color);
                std::shared_ptr<Scene::CLightComponent> createSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius);
                std::shared_ptr<Scene::CLightComponent> createSource(Resources::ELightSourceType eType, FTransform transform, glm::vec3 color, float attenuation_radius);

                std::vector<Core::FLight> getSources();
            private:
                std::vector<std::shared_ptr<Scene::CLightComponent>> m_vLights;
            };            
        }
    }
}