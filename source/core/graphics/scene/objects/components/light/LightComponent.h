#pragma once
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/buffer/VulkanUniform.h"

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightComponent : public CRenderObject
            {
            public:
                CLightComponent();
                void create() override;
                void reCreate() override;
                void update(float fDeltaTime) override;
                void render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;

                glm::vec3 getColor() const { return m_vColor; }
                float getAttenuation() { return m_fAttenuation; }
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