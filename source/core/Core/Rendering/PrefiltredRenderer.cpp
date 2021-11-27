#include "PrefiltredRenderer.h"
#include "Resources/Textures/Image.h"
//#include "Resources/Materials/MaterialIrradiance.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"
#include "Resources/Meshes/Primitives.hpp"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

void PrefiltredRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}

void PrefiltredRenderer::ReCreate(uint32_t framesInFlight)
{

}

void PrefiltredRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    auto pLocalVBO = std::make_unique<VulkanVBO>();
    auto vertices = FCube::m_vVertices;
    auto indices = FCube::m_vIndices;
    pLocalVBO->AddMeshData(std::move(vertices), std::move(indices));
    pLocalVBO->Create();
    pLocalVBO->Bind(commandBuffer);

    BeginRender(commandBuffer);
    EndRender(commandBuffer);
}

void PrefiltredRenderer::Cleanup()
{

}

void PrefiltredRenderer::CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}