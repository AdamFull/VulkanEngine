#pragma once
#include "graphics/pipeline/Pipeline.h"

namespace Engine
{
    namespace Core
    {
        class VulkanBuffer;
        class PushHandler : public utl::non_copy_movable
        {
        public:
            void Create(const Pipeline::CPushConstBlock &pushBlock);
            void ReCreate();
            void Cleanup();
            void Flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::CPipelineBase> pPipeline);

            template<class T>
            void Set(const T &object, uint32_t index, std::size_t offset, std::size_t size)
            {
                std::memcpy(m_vData.at(index).get() + offset, &object, size);
            }

            template<class T>
            void Set(const std::string &uniformName, const T &object, uint32_t index, std::size_t size = 0)
            {
                if (!m_pushBlock)
			        return;

                auto uniform = m_pushBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform->getSize()));
                
                Set(object, index, static_cast<std::size_t>(uniform->getOffset()), realSize);
            }

        private:
            std::optional<Pipeline::CPushConstBlock> m_pushBlock;
            std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
            std::vector<std::unique_ptr<char[]>> m_vData;
        };
    }
}