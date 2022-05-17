#pragma once
#include "resources/ResourceCunstruct.h"
#include "graphics/scene/objects/components/light/LightSourcePoint.h"
#include "graphics/scene/objects/components/light/LightSourceDirectional.h"
#include "graphics/scene/objects/components/light/LightSourceSpot.h"
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            class CLightSourceManager : public utl::singleton<CLightSourceManager>
            {
            public:
                template<class _Ty>
                std::shared_ptr<_Ty> createSource() {}

                template<>
                std::shared_ptr<Scene::CLightSourcePoint> createSource()
                {
                    auto pNewSource = std::make_shared<Scene::CLightSourcePoint>();
                    std::stringstream ss{};
                    ss << "PointLight_" << vPointLights.size();
                    pNewSource->setName(ss.str());
                    vPointLights.emplace_back(pNewSource);
                    return vPointLights.back();
                }

                template<>
                std::shared_ptr<Scene::CLightSourceDirectional> createSource()
                {
                    auto pNewSource = std::make_shared<Scene::CLightSourceDirectional>();
                    std::stringstream ss{};
                    ss << "DirectionalLight_" << vDirectionalLights.size();
                    pNewSource->setName(ss.str());
                    vDirectionalLights.emplace_back(pNewSource);
                    return vDirectionalLights.back();
                }

                template<>
                std::shared_ptr<Scene::CLightSourceSpot> createSource()
                {
                    auto pNewSource = std::make_shared<Scene::CLightSourceSpot>();
                    std::stringstream ss{};
                    ss << "SpotLight_" << vSpotLights.size();
                    pNewSource->setName(ss.str());
                    vSpotLights.emplace_back(pNewSource);
                    return vSpotLights.back();
                }

                template<class _Ty>
                std::vector<_Ty> getSources(){}

                template<>
                std::vector<FPointLight> getSources()
                {
                    std::vector<FPointLight> vOutput{};
                    for(auto& source : vPointLights)
                    {
                        if(source->isEnabled() && source->wasRendered())
                            vOutput.emplace_back(source->getLight());
                    }
                    return vOutput;
                }

                template<>
                std::vector<FDirectionalLight> getSources()
                {
                    std::vector<FDirectionalLight> vOutput{};
                    for(auto& source : vDirectionalLights)
                    {
                        if(source->isEnabled() && source->wasRendered())
                            vOutput.emplace_back(source->getLight());
                    }
                    return vOutput;
                }

                template<>
                std::vector<FSpotLight> getSources()
                {
                    std::vector<FSpotLight> vOutput{};
                    for(auto& source : vSpotLights)
                    {
                        if(source->isEnabled() && source->wasRendered())
                            vOutput.emplace_back(source->getLight());
                    }
                    return vOutput;
                }

            private:
                std::vector<std::shared_ptr<Scene::CLightSourcePoint>> vPointLights;
                std::vector<std::shared_ptr<Scene::CLightSourceDirectional>> vDirectionalLights;
                std::vector<std::shared_ptr<Scene::CLightSourceSpot>> vSpotLights;
            };            
        }
    }
}