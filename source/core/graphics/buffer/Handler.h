#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "VulkanBuffer.h"

namespace engine
{
    namespace core
    {
        enum class EHandlerStatus
        {
            eNone,
            eChanged,
            eFlushed
        };

        class CHandler : public utl::non_copy_movable
        {
        public:
            CHandler() = default;
            virtual ~CHandler();
            virtual void create(const pipeline::CUniformBlock &uniformBlock);
            void reCreate();
            void cleanup();
            void flush();

            template<class T>
            void write(T& object)
            {
                if (!uniformBlock || vBuffers.empty())
			        return;
                
                auto index = getCurrentFrameProxy();
                auto& pBuffer = vBuffers.at(index);

                if(!vMapped.at(index))
                {
                    pBuffer->mapMem();
                    vMapped.at(index) = true;
                }

                auto size = sizeof(object);
                if(status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, 0))
                {
                    pBuffer->write((void*)&object, size, 0);
                    status = EHandlerStatus::eChanged;
                }

                flush();
            }

            template<class T>
            void set(T& object, std::size_t offset, std::size_t size)
            {
                if (!uniformBlock || vBuffers.empty())
			        return;

                auto index = getCurrentFrameProxy();
                auto& pBuffer = vBuffers.at(index);

                if(!vMapped.at(index))
                {
                    pBuffer->mapMem();
                    vMapped.at(index) = true;
                }

                if(status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, offset))
                {
                    pBuffer->write((void*)&object, size, offset);
                    status = EHandlerStatus::eChanged;
                }
            }

            template<class T>
            void set(const std::string &uniformName, const T& object, std::size_t size = 0)
            {
                if (!uniformBlock)
			        return;

                auto uniform = uniformBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = uniform->getSize();

                auto testSize = sizeof(object);
                
                set(object, uniform->getOffset(), realSize);
            }

            // Getters
            scope_ptr<CVulkanBuffer>& getBuffer();
        private:
            uint32_t getCurrentFrameProxy();
        protected:
            std::optional<pipeline::CUniformBlock> uniformBlock;
            std::vector<scope_ptr<CVulkanBuffer>> vBuffers;
            std::vector<bool> vMapped;
            bool bIsClean{false};
            EHandlerStatus status{EHandlerStatus::eNone};
        };
    }
}