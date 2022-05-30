#pragma once
#include "MeshFragment.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
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

            /**
             * @brief Base class used for mesh representation 
             * 
             */
            class CMeshBase
            {
            public:
                /**
                 * @brief Creates mesh object and all attached sub objects
                 * 
                 */
                virtual void create();

                /**
                 * @brief Calling recreate for all sub objects
                 * 
                 */
                virtual void reCreate();

                /**
                 * @brief Rendering mesh and all attached sub objects
                 * 
                 * @param commandBuffer Current command buffer
                 * @param ubo Uniform buffer data
                 * @param instanceCount Draw instances count
                 */
                virtual void render(vk::CommandBuffer commandBuffer, const glm::mat4& model, uint32_t instanceCount = 1);

                /**
                 * @brief Cleans all attached objects in mesh
                 * 
                 */
                virtual void cleanup();

                /**
                 * @brief Set texture repeat times
                 * 
                 * @param repeatTimes Repeat times
                 */
                inline void textureRepeat(float repeatTimes) { fRepeat = repeatTimes; }

                /**
                 * @brief Add mesh fragment
                 * 
                 * @param fragment Mesh base fragment
                 */
                void addFragment(std::shared_ptr<CMeshFragment> fragment);

                /**
                 * @brief Get the Animations object
                 * 
                 * @return std::vector<FAnimation>& Animations reference vector
                 */
                inline std::vector<FAnimation>& getAnimations() { return m_vAnimations; }

                /**
                 * @brief Add new animation
                 * 
                 * @param animation Animation object
                 */
                inline void addAnimation(FAnimation&& animation) { m_vAnimations.emplace_back(animation); }

                /**
                 * @brief Set mesh name
                 * 
                 * @param srName 
                 */
                void setName(const std::string& srName);

                /**
                 * @brief Returns mesh name
                 * 
                 * @return std::string Mesh name in string
                 */
                inline std::string getName() { return m_srName; }

            protected:
                std::vector<std::shared_ptr<CMeshFragment>> m_vFragments;
                std::vector<FAnimation> m_vAnimations;
                std::string m_srName;
                float fRepeat;
            };
        }
    }
}