#pragma once
#include "resources/materials/VulkanMaterial.h"
#include "graphics/buffer/VulkanBuffer.h"
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;

        namespace Mesh
        {
            struct Primitive
            {
                uint32_t firstIndex;
                uint32_t indexCount;
                uint32_t firstVertex;
                uint32_t vertexCount;
                bool bUseMaterial{false};
                std::shared_ptr<Material::MaterialBase> material;

                struct Dimensions
                {
                    glm::vec3 min = glm::vec3(FLT_MAX);
                    glm::vec3 max = glm::vec3(-FLT_MAX);
                    glm::vec3 size;
                    glm::vec3 center;
                    float radius;
                } dimensions;

                void setDimensions(glm::vec3 min, glm::vec3 max);
            };

            class MeshFragment
            {
            public:
                
                void Create(vk::RenderPass& renderPass, uint32_t subpass);
                virtual void AddPrimitive(Primitive &&primitive);
                Primitive& GetPrimitive(uint32_t index);
                virtual void SetMaterial(std::shared_ptr<Material::MaterialBase> material);
                void ReCreate();
                void Update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex, uint32_t instanceCount = 1);
                void Cleanup();
                void Destroy();

                inline void SetLocalMatrix(glm::mat4 matrix) { m_mMatrix = matrix; }
                inline glm::mat4& GetLocalMatrix() { return m_mMatrix; }

                void SetName(const std::string& srName);
                inline std::string GetName() { return m_srName; }

            protected:
                std::vector<Primitive> m_vPrimitives;
                glm::mat4 m_mMatrix;
                std::string m_srName;
            };
        }
    }
}