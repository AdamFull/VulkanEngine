#pragma once

namespace Core
{
    namespace Render
    {
        class Image;
        class CFramebuffer
        {
        public:
            class Builder
            {
            public:
                std::unique_ptr<CFramebuffer> build(vk::RenderPass& renderPass);
            private:
            };

            CFramebuffer() = default;
        private:
            std::vector<vk::Framebuffer> vFramebuffers;
            std::map<uint32_t, std::vector<Image>> mImages;
        };
    }
}