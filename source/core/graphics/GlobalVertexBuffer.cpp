#include "GlobalVertexBuffer.h"

using namespace Engine::Core;

std::unique_ptr<CVBO> CVBO::_instance{nullptr};

CVBO::CVBO()
{
    pVBO = std::make_unique<CVertexBufferObject>();
}

std::unique_ptr<CVertexBufferObject>& CVBO::getInstance()
{
    if(!_instance)
        _instance.reset(new CVBO());
    return _instance->pVBO;
}