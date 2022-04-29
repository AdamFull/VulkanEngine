#pragma once
#include "graphics/scene/objects/RenderObject.h"

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
                    class ComponentBase : public RenderObject
                    {
                    public:
                        void Create(vk::RenderPass& renderPass, uint32_t subpass) override;
                        void ReCreate() override;
                        void Update(float fDeltaTime) override;
                        void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                        void Cleanup() override;
                        void Destroy() override;
                    };
                }
            }
        }
    }
}