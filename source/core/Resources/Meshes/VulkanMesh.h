#pragma once
#include "Resources/ResourceBase.h"
#include "Renderer/VulkanBuffer.h"
#include "Renderer/DataTypes/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            class MaterialBase;
        }

        namespace Mesh
        {
            struct Primitive
            {
                uint32_t firstIndex;
                uint32_t indexCount;
                uint32_t firstVertex;
                uint32_t vertexCount;
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

            class MeshBase : public ResourceBase
            {
            public:
                void Create() override;
                virtual void AddPrimitive(Primitive &&primitive);
                virtual void SetMaterial(std::shared_ptr<Material::MaterialBase> material);
                void ReCreate() override;
                void Update(uint32_t imageIndex) override;
                void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

            protected:
                std::vector<Primitive> m_vPrimitives;
            };
        }
    }
}