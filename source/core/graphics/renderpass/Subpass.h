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

                virtual void create(vk::RenderPass& renderPass) {}
                virtual void render(vk::CommandBuffer& commandBuffer) {}
            private:
            };
        }
    }
}