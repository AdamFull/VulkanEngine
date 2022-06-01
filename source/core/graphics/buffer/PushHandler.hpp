#pragma once
#include "graphics/pipeline/Pipeline.h"
#include <util/helpers.hpp>

namespace engine
{
    namespace core
    {
        class CVulkanBuffer;
        class CPushHandler : public utl::non_copy_movable
        {
        public:
            CPushHandler() = default;
            ~CPushHandler();
            void create(const pipeline::CPushConstBlock &pushBlock, ref_ptr<pipeline::CPipelineBase>& pipeline);
            void reCreate(ref_ptr<pipeline::CPipelineBase>& pipeline);
            void cleanup();
            void flush(vk::CommandBuffer& commandBuffer);

            template<class T>
            void set(const T &object, std::size_t offset, std::size_t size)
            {
                auto index = getCurrentFrameProxy();
                std::memcpy(vData.at(index).get() + offset, &object, size);
            }

            template<class T>
            void set(const std::string &uniformName, const T &object, std::size_t size = 0)
            {
                if (!pushBlock)
			        return;

                auto uniform = pushBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), uniform->getSize());
                
                set(object, uniform->getOffset(), realSize);
            }

        private:
            uint32_t getCurrentFrameProxy();

            std::optional<pipeline::CPushConstBlock> pushBlock;
            std::vector<scope_ptr<CVulkanBuffer>> vBuffers;
            std::vector<scope_ptr<char[]>> vData;
            ref_ptr<pipeline::CPipelineBase> pPipeline;
            bool bIsClean{false};
        };
    }
}