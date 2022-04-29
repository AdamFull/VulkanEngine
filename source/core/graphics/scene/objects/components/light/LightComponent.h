#pragma once
#include "graphics/scene/objects/components/ComponentBase.h"
#include "graphics/buffer/VulkanUniform.h"

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
                    namespace Light
                    {
                        class LightComponent : public ComponentBase
                        {
                        public:
                            void Create(vk::RenderPass& renderPass, uint32_t subpass) override;
                            void ReCreate() override;
                            void Update(float fDeltaTime) override;
                            void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;

                            inline void SetColor(glm::vec3 color) { m_vColor = color; }
                            inline void SetAttenuation(float attenuation) { m_fAttenuation = attenuation; }

                            FLight GetLight();

                        protected:
                            glm::vec3 m_vColor{};
                            float m_fAttenuation;
                        };
                    }
                }
            }
        }
    }
}