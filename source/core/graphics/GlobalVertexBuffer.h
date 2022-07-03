#pragma once
#include "util/helpers.hpp"
#include "graphics/buffer/VulkanVBO.h"

namespace engine
{
    namespace core
    {
        // Just a proxy class for handle one instance of global vbo
        class CVBO : public utl::non_copy_movable
        {
        protected:
            static utl::scope_ptr<CVBO> _instance;

        public:
            CVBO();
            static utl::scope_ptr<CVertexBufferObject> &inst();

        private:
            utl::scope_ptr<CVertexBufferObject> pVBO;
        };
    }
}