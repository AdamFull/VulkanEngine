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
                    class SceneRootComponent : public RenderObject
                    {
                    public:
                        explicit SceneRootComponent(std::string srName = "SceneRoot")
                        {
                            m_srName = srName;
                        }

                        void Create(vk::RenderPass& renderPass, uint32_t subpass) override;
                        void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                        void Update(float fDeltaTime) override;
                    };
                }
            }
        }
    }
}