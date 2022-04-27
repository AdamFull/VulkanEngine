#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(vk::RenderPass& renderPass, uint32_t subpass) {}
                virtual void render(vk::CommandBuffer& commandBuffer) {}
            private:
            };
        }
    }
}