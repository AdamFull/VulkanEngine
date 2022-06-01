#include "LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"

//TODO: move managers to another space

using namespace engine::core;
using namespace engine::core::scene;

template<>
scope_ptr<CLightSourceManager> utl::singleton<CLightSourceManager>::_instance{nullptr};