#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "Handler.h"
#include <util/helpers.hpp>

namespace engine
{
    namespace core
    {
        class CStorageHandler : public CHandler
        {
        public:
            CStorageHandler() = default;
            void create(const pipeline::CUniformBlock &uniformBlock) override;
        };
    }
}