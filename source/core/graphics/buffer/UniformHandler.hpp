#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "Handler.h"

namespace engine
{
    namespace core
    {
        class CUniformHandler : public CHandler
        {
        public:
            CUniformHandler() = default;
            CUniformHandler(const pipeline::CUniformBlock &uniformBlock);
        private:
            void create(const pipeline::CUniformBlock &uniformBlock) override;
        };
    }
}