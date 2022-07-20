#pragma once
#include "graphics/pipeline/Pipeline.h"

namespace engine
{
    namespace core
    {
        class CVulkanBuffer;
        class CPushHandler : public utl::non_copy_movable
        {
        public:
            CPushHandler() = default;
            CPushHandler(const pipeline::CPushConstBlock &pushBlock, utl::ref_ptr<pipeline::CPipelineBase>& pipeline);
            ~CPushHandler();
            void reCreate(utl::ref_ptr<pipeline::CPipelineBase>& pipeline);
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
            void create(const pipeline::CPushConstBlock &pushBlock, utl::ref_ptr<pipeline::CPipelineBase>& pipeline);
            uint32_t getCurrentFrameProxy();

            std::optional<pipeline::CPushConstBlock> pushBlock;
            std::vector<utl::scope_ptr<char[]>> vData;
            utl::ref_ptr<pipeline::CPipelineBase> pPipeline;
        };
    }
}