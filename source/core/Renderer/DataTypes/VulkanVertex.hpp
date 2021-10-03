#pragma once
#include <imgui.h>

namespace Engine
{
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

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;

            return bindingDescription;
        }

        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
            attributeDescriptions.resize(4);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
            attributeDescriptions[2].offset = offsetof(Vertex, normal);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = vk::Format::eR32G32Sfloat;
            attributeDescriptions[3].offset = offsetof(Vertex, texcoord);

            return attributeDescriptions;
        }
    };

    struct VertexUI
    {
        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(ImDrawVert);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;

            return bindingDescription;
        }

        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
            attributeDescriptions.resize(3);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
            attributeDescriptions[0].offset = offsetof(ImDrawVert, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = vk::Format::eR32G32Sfloat;
            attributeDescriptions[1].offset = offsetof(ImDrawVert, uv);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = vk::Format::eR8G8B8A8Unorm;
            attributeDescriptions[2].offset = offsetof(ImDrawVert, col);

            return attributeDescriptions;
        }
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