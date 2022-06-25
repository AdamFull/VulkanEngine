#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/scene/objects/components/LightComponentDirectional.h"
#include "graphics/scene/objects/components/LightComponentPoint.h"
#include "graphics/scene/objects/components/LightComponentSpot.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            using directional_lights_array = std::array<scene::FDirectionalLight, MAX_DIRECTIONAL_LIGHT_COUNT>;
            using point_lights_array = std::array<scene::FPointLight, MAX_POINT_LIGHT_COUNT>;
            using spot_lights_array = std::array<scene::FSpotLight, MAX_SPOT_LIGHT_COUNT>;

            class CRenderObject;
            class CLightSourceManager : public utl::singleton<CLightSourceManager>
            {
            public:
                void addLight(ref_ptr<CRenderObject>& light);
                directional_lights_array& getDirectionalSources(uint32_t& light_count);
                point_lights_array& getPointSources(uint32_t& light_count);
                spot_lights_array& getSpotSources(uint32_t& light_count);
                std::vector<ref_ptr<CRenderObject>>& getObjects() { return vLights; }
            private:
                std::vector<ref_ptr<CRenderObject>> vLights;
                directional_lights_array directionalLights;
                point_lights_array pointLights;
                spot_lights_array spotLights;
            };
        }
    }
}