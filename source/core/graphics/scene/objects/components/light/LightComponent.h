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

                glm::vec3 getColor() const { return vColor; }
                float getAttenuation() { return fAttenuation; }
                float getIntencity() { return fIntencity; }
                inline void setColor(glm::vec3 color) { vColor = color; }
                inline void setAttenuation(float attenuation) { fAttenuation = attenuation; }
                inline void setIntencity(float intencity) { fIntencity = intencity; }

                FLight getLight();

            protected:
                glm::vec3 vColor{};
                float fAttenuation;
                float fIntencity{1.f};
            };
        }
    }
}