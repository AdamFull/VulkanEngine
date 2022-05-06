#include "MeshFragment.h"
#include "util/uuid.hpp"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/VulkanHighLevel.h"

using namespace Engine::Resources::Mesh;

void FPrimitive::setDimensions(glm::vec3 min, glm::vec3 max)
{
    dimensions.min = min;
    dimensions.max = max;
    dimensions.size = max - min;
    dimensions.center = (min + max) / 2.0f;
    dimensions.radius = glm::distance(min, max) / 2.0f;
}

void CMeshFragment::create(vk::RenderPass& renderPass, uint32_t subpass)
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->create(renderPass, subpass);
    }
}

void CMeshFragment::addPrimitive(FPrimitive &&primitive)
{
    m_vPrimitives.emplace_back(primitive);
}

FPrimitive& CMeshFragment::getPrimitive(uint32_t index)
{
    return m_vPrimitives.at(index);
}

void CMeshFragment::setMaterial(std::shared_ptr<Material::CMaterialBase> material)
{
    /*auto it = m_vPrimitives.find(srPrimitiveName);
    std::find(m_vPrimitives.begin(), m_vPrimitives.end(), )
    if(it != m_vPrimitives.end())
        it->second.material = material;
    else
        assert(false && "Primitive for material was not found");*/
}

void CMeshFragment::reCreate()
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->reCreate();
    }
}

void CMeshFragment::update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex)
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material && primitive.bUseMaterial)
        {
             primitive.material->addBuffer("FUniformData", uboDesc);
            primitive.material->update(imageIndex);
        }
    }
}

void CMeshFragment::bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex, uint32_t instanceCount)
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material && primitive.bUseMaterial)
            primitive.material->bind(commandBuffer, imageIndex);
            
        commandBuffer.drawIndexed(primitive.indexCount, instanceCount, primitive.firstIndex, 0, 0);
    }
}

void CMeshFragment::cleanup()
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->cleanup();
    }
}

void CMeshFragment::destroy()
{

}

void CMeshFragment::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}