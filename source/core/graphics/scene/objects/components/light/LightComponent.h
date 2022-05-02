#pragma once
#include "graphics/scene/objects/components/ComponentBase.h"
#include "graphics/buffer/VulkanUniform.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightComponent : public CComponentBase
            {
            public:
                void create(vk::RenderPass &renderPass, uint32_t subpass) override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;

                inline void setColor(glm::vec3 color) { m_vColor = color; }
                inline void setAttenuation(float attenuation) { m_fAttenuation = attenuation; }

                FLight getLight();

            protected:
                glm::vec3 m_vColor{};
                float m_fAttenuation;
            };
        }
    }
}