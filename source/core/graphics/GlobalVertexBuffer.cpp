#include "GlobalVertexBuffer.h"

using namespace engine::core;

scope_ptr<CVBO> CVBO::_instance{nullptr};

CVBO::CVBO()
{
    pVBO = make_scope<CVertexBufferObject>();
}

scope_ptr<CVertexBufferObject>& CVBO::inst()
{
    if(!_instance)
        _instance.reset(new CVBO());
    return _instance->pVBO;
}