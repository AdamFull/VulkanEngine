#pragma once
#include <graphics/pipeline/Pipeline.h>
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        class CVulkanBuffer;
        class CUniformHandler : public utl::non_copy_movable
        {
        public:
            void create(const Pipeline::CUniformBlock &uniformBlock);
            void reCreate();
            void cleanup();
            void flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::CPipelineBase> pPipeline);

            template<class T>
            void set(const T &object, uint32_t index, std::size_t offset, std::size_t size)
            {
                if (!uniformBlock || vBuffers.empty())
			        return;

                if(!vMapped.at(index))
                {
                    vBuffers.at(index)->mapMem();
                    vMapped.at(index) = true;
                }

                auto* data = vBuffers.at(index)->getMappedMemory();

                std::memcpy(static_cast<char *>(data) + offset, &object, size);
            }

            template<class T>
            void set(const std::string &uniformName, const T &object, uint32_t index, std::size_t size = 0)
            {
                if (!uniformBlock)
			        return;

                auto uniform = uniformBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform->getSize()));
                
                set(object, index, static_cast<std::size_t>(uniform->getOffset()), realSize);
            }

            // Getters
            std::vector<std::unique_ptr<CVulkanBuffer>> &getUniformBuffers();
            std::unique_ptr<CVulkanBuffer> &getUniformBuffer(uint32_t index);
        private:
            void createUniformBuffers(uint32_t inFlightFrames);
            
            std::optional<Pipeline::CUniformBlock> uniformBlock;
            size_t iUniformSize{0};
            std::vector<std::unique_ptr<CVulkanBuffer>> vBuffers;
            std::vector<bool> vMapped;
        };
    }
}