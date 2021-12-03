#include "LightSourceManager.h"
#include "Core/Scene/Objects/Components/Light/LightComponent.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Scene::Objects::Components::Light;

std::unique_ptr<LightSourceManager> LightSourceManager::m_pInstance{nullptr};

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(ELightSourceType eType, glm::vec3 position)
{
    auto pNewSource = std::make_shared<LightComponent>();
    pNewSource->SetName("LightSource");
    pNewSource->SetPosition(position);
    m_vLights.emplace_back(std::move(pNewSource));
    return m_vLights.back();
}

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(ELightSourceType eType, glm::vec3 position, glm::vec3 color)
{
    auto pNewSource = std::make_shared<LightComponent>();
    pNewSource->SetName("LightSource");
    pNewSource->SetPosition(position);
    pNewSource->SetColor(color);
    m_vLights.emplace_back(std::move(pNewSource));
    return m_vLights.back();
}

std::shared_ptr<LightComponent> LightSourceManager::CreateSource(ELightSourceType eType, glm::vec3 position, glm::vec3 color, float attenuation_radius)
{
    auto pNewSource = std::make_shared<LightComponent>();
    pNewSource->SetName("LightSource");
    pNewSource->SetPosition(position);
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