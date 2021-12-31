#include "IrradianceRenderer.h"
#include "Core/Image/Image.h"
//#include "Resources/Materials/MaterialIrradiance.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"
#include "Resources/Meshes/Primitives.hpp"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
;
using namespace Engine::Resources::Material;

void IrradianceRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}

void IrradianceRenderer::ReCreate(uint32_t framesInFlight)
{

}

void IrradianceRenderer::Render(vk::CommandBuffer& commandBuffer)
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

void IrradianceRenderer::Cleanup()
{

}

void IrradianceRenderer::CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}