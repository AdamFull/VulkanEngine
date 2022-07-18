#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "Handler.h"

namespace engine
{
    namespace core
    {
        class CStorageHandler : public CHandler
        {
        public:
            CStorageHandler() = default;
            CStorageHandler(const pipeline::CUniformBlock &uniformBlock);
        protected:
            void create(const pipeline::CUniformBlock &uniformBlock) override;
        };
    }
}