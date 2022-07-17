#include "Subpass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"

using namespace engine::core;
using namespace engine::core::render;
using namespace engine::resources;
using namespace engine::resources::material;

void CSubpass::create()
{
    //pMaterial = CMaterialLoader::inst()->create(srShader);
    //for(auto& [name, define] : mDefinitions)
    //    pMaterial->addDefinition(name, define);
    //pMaterial->create();
}

void CSubpass::reCreate()
{
    //pMaterial->reCreate();
}

void CSubpass::beforeRender(vk::CommandBuffer& commandBuffer)
{

}

void CSubpass::render(vk::CommandBuffer& commandBuffer)
{
    /*CRenderSystem::inst()->setStageType(eType);
    auto& images = CRenderSystem::inst()->getCurrentImages();
    for(const auto& attachment : vTextureAttachments)
        pMaterial->addTexture(attachment, images[attachment]);
    
    if(pUserDefinedCodeRender)
        pUserDefinedCodeRender(commandBuffer);*/
}

void CSubpass::cleanup()
{
    if(pMaterial)
        pMaterial->cleanup();
}