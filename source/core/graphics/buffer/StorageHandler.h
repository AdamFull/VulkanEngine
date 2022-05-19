#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "Handler.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        class CStorageHandler : public CHandler
        {
        public:
            CStorageHandler() = default;
            void create(const Pipeline::CUniformBlock &uniformBlock) override;
        };
    }
}