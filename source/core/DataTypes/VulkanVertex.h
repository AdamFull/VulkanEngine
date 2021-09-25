#pragma once

namespace Engine
{
    struct FUniformData 
    {
        glm::mat4 transform{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    struct FTransform
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
    struct hash<Engine::Vertex>
    {
        size_t operator()(Engine::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}