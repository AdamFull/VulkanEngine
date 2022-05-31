#pragma once
#include "util/helpers.hpp"
#include "graphics/buffer/VulkanVBO.h"

namespace Engine
{
    namespace Core
    {
        //Just a proxy class for handle one instance of global vbo
        class CVBO : public utl::non_copy_movable
        {
        protected:
            static scope_ptr<CVBO> _instance;
        public:
            CVBO();
            static scope_ptr<CVertexBufferObject>& inst();
        private:
            scope_ptr<CVertexBufferObject> pVBO;
        };
    }
}