#pragma once

namespace Engine
{
    namespace Types
    {
        struct FUniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        struct FVulkanTransform
        {
            glm::vec3 pos;
            glm::vec3 rot;
            glm::vec3 scale;
        };

        struct FVertex
        {
            glm::vec3 pos;
            glm::vec4 color;
            glm::vec2 texCoord;

            bool operator==(const FVertex& other) const 
            {
                return pos == other.pos && color == other.color && texCoord == other.texCoord;
            }
        };

        vk::VertexInputBindingDescription GetBindingDescription();
        std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions();
    }
}

namespace std
{
    template <>
    struct hash<Engine::Types::FVertex>
    {
        size_t operator()(Engine::Types::FVertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}