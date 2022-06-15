#pragma once
#include "SceneComponent.h"

namespace engine
{
    namespace resources
    {
        namespace material
        {
            class CMaterialBase;
        }
    }
    namespace core
    {
        namespace scene
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
                ref_ptr<resources::material::CMaterialBase> material;

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
             * @brief Animation channel description
             * 
             */
            struct FAnimationChannel 
            {
                /// @brief Channel path type
                enum class EPathType { TRANSLATION, ROTATION, SCALE };
                EPathType path;
                //Node* node;
                /// @brief Animation sampler index
                uint32_t samplerIndex;
            };

            /**
             * @brief Animation sampler
             * 
             */
            struct FAnimationSampler 
            {
                /// @brief Animation interpolation type
                enum class EInterpolationType { LINEAR, STEP, CUBICSPLINE };
                EInterpolationType interpolation;
                /// @brief Sampler inputs
                std::vector<float> inputs;
                /// @brief Output positons
                std::vector<glm::vec4> outputsVec4;
            };

            /**
             * @brief Mesh animation
             * 
             */
            struct FAnimation 
            {
                /// @brief Animation name
                std::string name;
                /// @brief Animation samplers
                std::vector<FAnimationSampler> samplers;
                /// @brief Animation channels
                std::vector<FAnimationChannel> channels;
                float start = std::numeric_limits<float>::max();
                float end = std::numeric_limits<float>::min();
            };
            
            class CMeshComponent;

            struct FSkin
            {
                std::string name;
                ref_ptr<CMeshComponent> skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMatrices;
                std::vector<ref_ptr<CMeshComponent>> joints;
            };

            class CMeshComponent : public CSceneComponent
            {
            public:
                void create() override;
                void reCreate() override;
                void render(vk::CommandBuffer &commandBuffer) override;
                void update(float fDeltaTime) override;
                void cleanup() override;
                void destroy() override;

                void addPrimitive(FPrimitive &&primitive);
            protected:
                std::vector<glm::vec4> vInstances;
                std::vector<FPrimitive> vPrimitives;
            };
        }
    }
}