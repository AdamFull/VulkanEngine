#include "GlobalVertexBuffer.h"

using namespace engine::core;

utl::scope_ptr<CVBO> CVBO::_instance{nullptr};

CVBO::CVBO()
{
    pVBO = utl::make_scope<CVertexBufferObject>();
}

utl::scope_ptr<CVertexBufferObject>& CVBO::inst()
{
    if(!_instance)
        _instance.reset(new CVBO());
    return _instance->pVBO;
}