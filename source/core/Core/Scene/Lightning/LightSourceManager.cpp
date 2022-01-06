#include "LightSourceManager.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects::Components::Light;

template<>
std::unique_ptr<LightSourceManager> Singleton<LightSourceManager>::m_pInstance{nullptr};

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(Resources::ELightSourceType eType, glm::vec3 position)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return CreateSource(eType, transform, {1.f, 1.f, 1.f}, 20.f);
}

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return CreateSource(eType, transform, color, 20.f);
}

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(Resources::ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius)
{
    FTransform transform{position, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
    return CreateSource(eType, transform, color, attenuation_radius);
}

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(Resources::ELightSourceType eType, FTransform transform, glm::vec3 color, float attenuation_radius)
{
    auto pNewSource = std::make_shared<LightComponent>();
    std::stringstream ss{};
    ss << "LightSource_" << m_vLights.size();
    pNewSource->SetName(ss.str());
    
    pNewSource->SetTransform(transform);
    pNewSource->SetColor(color);
    pNewSource->SetAttenuation(attenuation_radius);
    m_vLights.emplace_back(std::move(pNewSource));
    return m_vLights.back();
}

std::vector<FLight> LightSourceManager::GetSources()
{
    std::vector<Core::FLight> vOutput{};
    for(auto& pLight : m_vLights)
        vOutput.emplace_back(pLight->GetLight());
    return vOutput;
}