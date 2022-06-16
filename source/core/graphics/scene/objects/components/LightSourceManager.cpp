#include "LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"

//TODO: move managers to another space

using namespace engine::core;
using namespace engine::core::scene;

template<>
scope_ptr<CLightSourceManager> utl::singleton<CLightSourceManager>::_instance{nullptr};

void CLightSourceManager::addLight(ref_ptr<CRenderObject>& light)
{
    vLights.emplace_back(light);
}

std::array<scene::FLight, max_light_count>& CLightSourceManager::getSources(uint32_t& light_count)
{
    light_count = 0;

    for(auto& light : vLights)
    {
        if(light->isEnabled())
        {
            auto& lightSource = light->getLight();
            if(light_count > max_light_count)
                break;

            lights[light_count] = lightSource->getLight();
            light_count++;
        }
    }

    return lights;
}