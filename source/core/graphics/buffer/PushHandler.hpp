#pragma once
#include "graphics/pipeline/Pipeline.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        class CVulkanBuffer;
        class CPushHandler : public utl::non_copy_movable
        {
        public:
            CPushHandler() = default;
            ~CPushHandler();
            void create(const Pipeline::CPushConstBlock &pushBlock);
            void reCreate();
            void cleanup();
            void flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::CPipelineBase> pPipeline);

            template<class T>
            void set(const T &object, uint32_t index, std::size_t offset, std::size_t size)
            {
                std::memcpy(vData.at(index).get() + offset, &object, size);
            }

            template<class T>
            void set(const std::string &uniformName, const T &object, uint32_t index, std::size_t size = 0)
            {
                if (!pushBlock)
			        return;

                auto uniform = pushBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform->getSize()));
                
                set(object, index, static_cast<std::size_t>(uniform->getOffset()), realSize);
            }

        private:
            std::optional<Pipeline::CPushConstBlock> pushBlock;
            std::vector<std::unique_ptr<CVulkanBuffer>> vBuffers;
            std::vector<std::unique_ptr<char[]>> vData;
        };
    }
}