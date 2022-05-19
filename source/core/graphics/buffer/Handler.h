#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "VulkanBuffer.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
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
            virtual void create(const Pipeline::CUniformBlock &uniformBlock);
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

                if(status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, offset))
                {
                    pBuffer->write((void*)&object, size, offset);
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
                auto aligned = sizeof(object);
                
                set(object, uniform->getOffset(), realSize);
            }

            // Getters
            std::unique_ptr<CVulkanBuffer>& getBuffer();
        private:
            uint32_t getCurrentFrameProxy();
        protected:
            std::optional<Pipeline::CUniformBlock> uniformBlock;
            std::vector<std::unique_ptr<CVulkanBuffer>> vBuffers;
            std::vector<bool> vMapped;
            bool bIsClean{false};
            EHandlerStatus status{EHandlerStatus::eNone};
        };
    }
}