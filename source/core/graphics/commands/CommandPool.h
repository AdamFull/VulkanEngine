#pragma once
#include <thread>

namespace engine
{
    namespace core
    {
        class CCommandPool
        {
        public:
            explicit CCommandPool(const std::thread::id &threadId = std::this_thread::get_id());
            ~CCommandPool();

            operator const vk::CommandPool &() const { return commandPool; }

            const vk::CommandPool &getCommandPool() const { return commandPool; }
            const std::thread::id &getThreadId() const { return threadId; }
        private:
            vk::CommandPool commandPool{VK_NULL_HANDLE};
	        std::thread::id threadId;
        };
    }
}