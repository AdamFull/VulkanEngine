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
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scale{1.f, 1.f, 1.f};
    };

    struct Vertex
    {
        glm::vec3 pos{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 texcoord{};

        bool operator==(const Vertex& other) const 
        {
            return pos == other.pos && color == other.color && normal == other.normal && texcoord == other.texcoord;
        }

        static vk::VertexInputBindingDescription getBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescriptions();
    };

    template <typename T, typename... Rest>
    void hashCombine(std::size_t &seed, const T &v, const Rest &...rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}

namespace std
{
    template <>
    struct hash<Engine::Vertex>
    {
        size_t operator()(Engine::Vertex const &vertex) const
        {
            size_t seed = 0;
            Engine::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texcoord);
            return seed;
        }
    };
}

/*namespace std
{
    template <>
    struct hash<Engine::Vertex>
    {
        size_t operator()(Engine::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
        }
    };
}*/