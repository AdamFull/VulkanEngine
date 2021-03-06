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
            void reCreate();
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
                    realSize = std::min(sizeof(object), uniform->getSize());

                auto testSize = sizeof(object);
                
                set(object, uniform->getOffset(), realSize);
            }

            // Getters
            utl::scope_ptr<CVulkanBuffer>& getBuffer();
        private:
            uint32_t getCurrentFrameProxy();
        protected:
            virtual void create(const pipeline::CUniformBlock &uniformBlock);

            std::optional<pipeline::CUniformBlock> uniformBlock;
            std::vector<utl::scope_ptr<CVulkanBuffer>> vBuffers;
            std::vector<bool> vMapped;
            EHandlerStatus status{EHandlerStatus::eNone};
        };
    }
}