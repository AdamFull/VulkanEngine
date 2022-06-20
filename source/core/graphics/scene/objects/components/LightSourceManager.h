#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/scene/objects/components/LightComponent.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CRenderObject;
            class CLightSourceManager : public utl::singleton<CLightSourceManager>
            {
            public:
                void addLight(ref_ptr<CRenderObject>& light);
                std::array<scene::FLight, max_light_count>& getSources(uint32_t& light_count);
                std::vector<ref_ptr<CRenderObject>>& getObjects() { return vLights; }
            private:
                std::vector<ref_ptr<CRenderObject>> vLights;
                std::array<scene::FLight, max_light_count> lights;
            };
        }
    }
}