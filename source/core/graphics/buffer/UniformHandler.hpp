#pragma once
#include <graphics/pipeline/Pipeline.h>
#include "Handler.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        class CUniformHandler : public CHandler
        {
        public:
            CUniformHandler() = default;
            void create(const Pipeline::CUniformBlock &uniformBlock) override;
        };
    }
}