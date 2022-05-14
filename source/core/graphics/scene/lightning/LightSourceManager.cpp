#include "LightSourceManager.h"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;

template<>
std::unique_ptr<CLightSourceManager> utl::singleton<CLightSourceManager>::_instance{nullptr};

std::shared_ptr<CLightComponent> CLightSourceManager::createSource(Resources::ELightSourceType eType, glm::vec3 position)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return createSource(eType, transform, {1.f, 1.f, 1.f}, 20.f);
}

std::shared_ptr<CLightComponent> CLightSourceManager::createSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return createSource(eType, transform, color, 20.f);
}

std::shared_ptr<CLightComponent> CLightSourceManager::createSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return createSource(eType, transform, color, attenuation_radius);
}

std::shared_ptr<CLightComponent> CLightSourceManager::createSource(Resources::ELightSourceType eType, FTransform transform, glm::vec3 color, float attenuation_radius)
{
    auto pNewSource = std::make_shared<CLightComponent>();
    std::stringstream ss{};
    ss << "LightSource_" << m_vLights.size();
    pNewSource->setName(ss.str());
    
    pNewSource->setTransform(transform);
    pNewSource->setColor(color);
    pNewSource->setAttenuation(attenuation_radius);
    m_vLights.emplace_back(std::move(pNewSource));
    return m_vLights.back();
}

std::vector<FLight> CLightSourceManager::getSources()
{
    std::vector<Core::FLight> vOutput{};
    for(auto& pLight : m_vLights)
    {
        if(pLight->isEnabled() && pLight->wasRendered())
            vOutput.emplace_back(pLight->getLight());
    }
    return vOutput;
}