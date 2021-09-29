#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class Device;

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

    class MeshBase : public ResourceBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;
    protected:
        virtual void CreateVertexBuffer(std::unique_ptr<Device>& device);
        virtual void CreateIndexBuffer(std::unique_ptr<Device>& device);

        std::vector<Vertex> vertices;
        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;

        std::vector<uint32_t> indices;
        vk::Buffer indexBuffer;
        vk::DeviceMemory indiciesBufferMemory;
    };

    class StaticMesh : public MeshBase
    {
    public:
        void Create(std::unique_ptr<Device>& device, std::shared_ptr<SwapChain> swapchain, std::shared_ptr<UniformBuffer> uniform) override;
        void ReCreate(std::unique_ptr<Device>& device) override;
        void Bind(std::unique_ptr<Device>& device, vk::CommandBuffer commandBuffer) override;
        void Destroy(std::unique_ptr<Device>& device) override;

        void Load(std::unique_ptr<Device>& device, std::string srPath);
        
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