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
            void set(T &object, std::size_t offset, std::size_t size)
            {
                if (!uniformBlock || !pBuffer)
			        return;

                if(!bMapped)
                {
                    pBuffer->mapMem();
                    bMapped = true;
                }


                if(status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, offset))
                {
                    pBuffer->write((void*)&object, size, offset);
                    status = EHandlerStatus::eChanged;
                }
            }

            template<class T>
            void set(const std::string &uniformName, const T &object, std::size_t size = 0)
            {
                if (!uniformBlock)
			        return;

                auto uniform = uniformBlock->getUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform->getSize()));
                
                set(object, static_cast<std::size_t>(uniform->getOffset()), realSize);
            }

            // Getters
            std::unique_ptr<CVulkanBuffer>& getBuffer() { return pBuffer; }
        protected:
            std::optional<Pipeline::CUniformBlock> uniformBlock;
            std::unique_ptr<CVulkanBuffer> pBuffer;
            bool bMapped{false};
            bool bIsClean{false};
            EHandlerStatus status{EHandlerStatus::eNone};
        };
    }
}