#include "SimpleCompositionRenderer.h"
#include "Resources/Textures/Image.h"
//#include "Resources/Materials/MaterialIrradiance.h"
#include "Resources/ResourceManager.h"
#include "Core/VulkanHighLevel.h"
#include "Core/VulkanInitializers.h"

using namespace Engine::Core;
using namespace Engine::Core::Rendering;
using namespace Engine::Resources;
using namespace Engine::Resources::Texture;
using namespace Engine::Resources::Material;

void SimpleCompositionRenderer::Create(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}

void SimpleCompositionRenderer::ReCreate(uint32_t framesInFlight)
{

}

void SimpleCompositionRenderer::Render(vk::CommandBuffer& commandBuffer)
{
    BeginRender(commandBuffer);
    EndRender(commandBuffer);
}

void SimpleCompositionRenderer::Cleanup()
{

}

void SimpleCompositionRenderer::CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr)
{

}