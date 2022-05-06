#pragma once
#include "resources/materials/VulkanMaterial.h"
#include "graphics/buffer/VulkanBuffer.h"
#include "graphics/data_types/VulkanVertex.hpp"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
            struct FPrimitive
            {
                uint32_t firstIndex;
                uint32_t indexCount;
                uint32_t firstVertex;
                uint32_t vertexCount;
                bool bUseMaterial{false};
                std::shared_ptr<Material::CMaterialBase> material;

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

            class CMeshFragment
            {
            public:
                
                void create();
                virtual void addPrimitive(FPrimitive &&primitive);
                FPrimitive& getPrimitive(uint32_t index);
                virtual void setMaterial(std::shared_ptr<Material::CMaterialBase> material);
                void reCreate();
                void update(vk::DescriptorBufferInfo& uboDesc, uint32_t imageIndex);
                void bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex, uint32_t instanceCount = 1);
                void cleanup();
                void destroy();

                inline void setLocalMatrix(glm::mat4 matrix) { m_mMatrix = matrix; }
                inline glm::mat4& getLocalMatrix() { return m_mMatrix; }

                void setName(const std::string& srName);
                inline std::string getName() { return m_srName; }

            protected:
                std::vector<FPrimitive> m_vPrimitives;
                glm::mat4 m_mMatrix;
                std::string m_srName;
            };
        }
    }
}