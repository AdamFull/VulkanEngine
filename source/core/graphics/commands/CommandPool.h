#pragma once
#include <thread>

namespace Engine
{
    namespace Core
    {
        class CommandPool
        {
        public:
            explicit CommandPool(const std::thread::id &threadId = std::this_thread::get_id());

            ~CommandPool();

            operator const vk::CommandPool &() const { return commandPool; }

            const vk::CommandPool &GetCommandPool() const { return commandPool; }
            const std::thread::id &GetThreadId() const { return threadId; }
        private:
            vk::CommandPool commandPool = VK_NULL_HANDLE;
	        std::thread::id threadId;
        };
    }
}