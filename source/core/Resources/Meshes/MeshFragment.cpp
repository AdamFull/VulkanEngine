#include "MeshFragment.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanHighLevel.h"

using namespace Engine::Resources::Mesh;

void Primitive::setDimensions(glm::vec3 min, glm::vec3 max)
{
    dimensions.min = min;
    dimensions.max = max;
    dimensions.size = max - min;
    dimensions.center = (min + max) / 2.0f;
    dimensions.radius = glm::distance(min, max) / 2.0f;
}

void MeshFragment::Create()
{
    for (auto &primitive : m_vPrimitives)
    {
        primitive.material->Create();
    }
}

void MeshFragment::AddPrimitive(Primitive &&primitive)
{
    m_vPrimitives.emplace_back(primitive);
}

Primitive& MeshFragment::GetPrimitive(uint32_t index)
{
    return m_vPrimitives.at(index);
}

void MeshFragment::SetMaterial(std::shared_ptr<Material::MaterialBase> material)
{
    /*auto it = m_vPrimitives.find(srPrimitiveName);
    std::find(m_vPrimitives.begin(), m_vPrimitives.end(), )
    if(it != m_vPrimitives.end())
        it->second.material = material;
    else
        assert(false && "Primitive for material was not found");*/
}

void MeshFragment::ReCreate()
{
    ResourceBase::ReCreate();
    for (auto &primitive : m_vPrimitives)
    {
        primitive.material->ReCreate();
    }
}

void MeshFragment::Update(uint32_t imageIndex)
{
    ResourceBase::Update(imageIndex);
    for (auto &primitive : m_vPrimitives)
    {
        primitive.material->Update(imageIndex);
    }
}

void MeshFragment::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    ResourceBase::Bind(commandBuffer, imageIndex);
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->Bind(commandBuffer, imageIndex);
            
        commandBuffer.drawIndexed(primitive.indexCount, 1, primitive.firstIndex, 0, 0);
    }
}

void MeshFragment::Cleanup()
{
    ResourceBase::Cleanup();
    for (auto &primitive : m_vPrimitives)
    {
        primitive.material->Cleanup();
    }
}

void MeshFragment::Destroy()
{
    ResourceBase::Destroy();
}
