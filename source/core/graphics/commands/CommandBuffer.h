#pragma once
#include "CommandPool.h"

namespace Engine
{
    namespace Core
    {
        class CCommandBuffer
        {
        public:
            explicit CCommandBuffer(bool _begin = true, vk::QueueFlagBits queueType = vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel bufferLevel = vk::CommandBufferLevel::ePrimary, uint32_t count = 1);
            ~CCommandBuffer();

            void begin(vk::CommandBufferUsageFlags usage = vk::CommandBufferUsageFlagBits::eOneTimeSubmit, uint32_t index = 0);
            void end();

            void submitIdle();

            vk::Result submit(uint32_t& imageIndex);

            operator const vk::CommandBuffer &() const { return vCommandBuffers.at(frameIndex); }

            vk::CommandBuffer &getCommandBuffer() { return vCommandBuffers.at(frameIndex); }
            bool isRunning() const { return running; }
        private:
            vk::Queue getQueue() const;
            uint32_t frameIndex{0};

            std::shared_ptr<CCommandPool> commandPool;

            vk::QueueFlagBits queueType;
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
            bool running = false;
        };
    }
}