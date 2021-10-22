#include "VulkanMesh.h"
#include "Resources/Materials/VulkanMaterial.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanHighLevel.h"

namespace Engine
{
    void MeshBase::Create()
    {
    }

    void MeshBase::AddPrimitive(std::string srPrimitiveName, Primitive&& primitive)
    {
        auto it = m_mPrimitives.find(srPrimitiveName);
        if(it != m_mPrimitives.end())
            assert(false && "Primitive with name: already exists!");
        else
            m_mPrimitives.emplace(srPrimitiveName, primitive);
    }

    void MeshBase::SetMaterial(std::string srPrimitiveName, std::shared_ptr<MaterialBase> material)
    {
        auto it = m_mPrimitives.find(srPrimitiveName);
        if(it != m_mPrimitives.end())
            it->second.material = material;
        else
            assert(false && "Primitive for material was not found");
    }

    void MeshBase::ReCreate()
    {
        ResourceBase::ReCreate();
        for(auto& [name, primitive] : m_mPrimitives)
        {
            primitive.material->ReCreate();
        }
    }

    void MeshBase::Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer)
    {
        ResourceBase::Update(imageIndex, pUniformBuffer);
        for(auto& [name, primitive] : m_mPrimitives)
        {
            primitive.material->Update(imageIndex, pUniformBuffer);
        }
    }

    void MeshBase::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
    {
        ResourceBase::Bind(commandBuffer, imageIndex);
        for(auto& [name, primitive] : m_mPrimitives)
        {
            primitive.material->Bind(commandBuffer, imageIndex);
            commandBuffer.drawIndexed(primitive.indexCount, 1, primitive.firstIndex, 0, 0);
        }
    }

    void MeshBase::Cleanup()
    {
        ResourceBase::Cleanup();
        for(auto& [name, primitive] : m_mPrimitives)
        {
            primitive.material->Cleanup();
        }
    }

    void MeshBase::Destroy()
    {
        ResourceBase::Destroy();
    }
}