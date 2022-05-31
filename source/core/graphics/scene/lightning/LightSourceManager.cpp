#include "LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;

template<>
scope_ptr<CLightSourceManager> utl::singleton<CLightSourceManager>::_instance{nullptr};