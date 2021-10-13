#pragma once
#include "VulkanMesh.h"

namespace Engine
{
    class StaticMesh : public MeshBase
    {
    public:
        void Create() override;
        void ReCreate() override;
        void Update(uint32_t imageIndex, std::unique_ptr<VulkanBuffer>& pUniformBuffer) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Cleanup() override;
        void Destroy() override;

        void CalcTangent();

        void Load(std::string srPath);
        
    };
}