#pragma once

namespace Engine::Types
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct VulkanTransform
    {
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scale;
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec4 color;
        glm::vec2 texCoord;

        bool operator==(const Vertex& other) const 
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }

        static vk::VertexInputBindingDescription getBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions();
    };
}

namespace std
{
    template <>
    struct hash<Render::Vertex>
    {
        size_t operator()(Render::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}