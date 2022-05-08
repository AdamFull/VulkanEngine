#pragma once
#include <thread>

namespace Engine
{
    namespace Core
    {
        class CCommandPool
        {
        public:
            explicit CCommandPool(const std::thread::id &threadId = std::this_thread::get_id());
            void cleanup();

            ~CCommandPool();

            operator const vk::CommandPool &() const { return commandPool; }

            const vk::CommandPool &getCommandPool() const { return commandPool; }
            const std::thread::id &getThreadId() const { return threadId; }
        private:
            vk::CommandPool commandPool = VK_NULL_HANDLE;
            bool bIsClean{false};
	        std::thread::id threadId;
        };
    }
}