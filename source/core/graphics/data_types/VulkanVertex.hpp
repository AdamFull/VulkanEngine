#pragma once
#include <imgui.h>

namespace Engine
{
    namespace Core
    {
        struct Vertex
        {
            Vertex() = default;
            Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), normal(n), texcoord(t) {}

            glm::vec3 pos{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 texcoord{};
            glm::vec4 tangent{};
            glm::vec4 joint0{};
            glm::vec4 weight0{};

            bool operator==(const Vertex &other) const
            {
                return pos == other.pos && color == other.color && normal == other.normal && texcoord == other.texcoord && tangent == other.tangent && joint0 == other.joint0 && weight0 == other.weight0;
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
                attributeDescriptions.resize(7);

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

                attributeDescriptions[4].binding = 0;
                attributeDescriptions[4].location = 4;
                attributeDescriptions[4].format = vk::Format::eR32G32B32A32Sfloat;
                attributeDescriptions[4].offset = offsetof(Vertex, tangent);

                attributeDescriptions[5].binding = 0;
                attributeDescriptions[5].location = 5;
                attributeDescriptions[5].format = vk::Format::eR32G32B32A32Sfloat;
                attributeDescriptions[5].offset = offsetof(Vertex, joint0);

                attributeDescriptions[6].binding = 0;
                attributeDescriptions[6].location = 6;
                attributeDescriptions[6].format = vk::Format::eR32G32B32A32Sfloat;
                attributeDescriptions[6].offset = offsetof(Vertex, weight0);

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
}

namespace std
{
    template <>
    struct hash<Engine::Core::Vertex>
    {
        size_t operator()(Engine::Core::Vertex const &vertex) const
        {
            size_t seed = 0;
            Engine::Core::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texcoord, vertex.tangent, vertex.joint0, vertex.weight0);
            return seed;
        }
    };
}