#include "Subpass.h"

using namespace Engine::Core::Render;

void CSubpass::cleanup()
{
    if(pUniform)
        pUniform->cleanup();
}