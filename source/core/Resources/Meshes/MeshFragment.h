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
            /**
             * @brief Mesh fragment that contain vertex and index positions and attached to fragment material
             * 
             */
            struct FPrimitive
            {
                uint32_t firstIndex;
                uint32_t indexCount;
                uint32_t firstVertex;
                uint32_t vertexCount;
                bool bUseMaterial{false};
                //TODO: check for material duplicates
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

            /**
             * @brief Mesh fragment is a part of mesh, that holds mesh fragments with attached materials
             * 
             */
            class CMeshFragment
            {
            public:
                /**
                 * @brief Call create for any attached primitives
                 * 
                 */
                void create();

                /**
                 * @brief Add new primitive
                 * 
                 * @param primitive Primitive object
                 */
                virtual void addPrimitive(FPrimitive &&primitive);

                /**
                 * @brief Get the Primitive object
                 * 
                 * @param index Primitive index
                 * @return FPrimitive& Primitive reference
                 */
                FPrimitive& getPrimitive(uint32_t index);

                /**
                 * @brief Call reCreation for attached primitives and included materials
                 * 
                 */
                void reCreate();

                /**
                 * @brief Binds materials attached to primitive 
                 * 
                 * @param commandBuffer Current command buffer
                 * @param instanceCount Number of instances to draw
                 */
                void render(vk::CommandBuffer commandBuffer, const glm::mat4& model, uint32_t instanceCount = 1);

                /**
                 * @brief Clena up all attached sub objects
                 * 
                 */
                void cleanup();

                /**
                 * @brief Set the Local Matrix of mesh fragment
                 * 
                 * @param matrix 
                 */
                inline void setLocalMatrix(glm::mat4 matrix) { m_mMatrix = matrix; }

                /**
                 * @brief Get the Local Matrix object
                 * 
                 * @return glm::mat4& 
                 */
                inline glm::mat4& getLocalMatrix() { return m_mMatrix; }

                /**
                 * @brief Set new fragment name
                 * 
                 * @param srName New fragment name
                 */
                void setName(const std::string& srName);

                /**
                 * @brief Get fragment name
                 * 
                 * @return std::string Fragment name
                 */
                inline std::string getName() { return m_srName; }

            protected:
                glm::vec4 instancePos[256];
                std::vector<FPrimitive> m_vPrimitives;
                glm::mat4 m_mMatrix;
                std::string m_srName;
            };
        }
    }
}