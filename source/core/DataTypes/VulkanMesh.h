#pragma once

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

    class VulkanMeshBase
    {
    public:
        std::vector<Vertex> vertices;
        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;

        std::vector<uint32_t> indices;
        vk::Buffer indexBuffer;
        vk::DeviceMemory indiciesBufferMemory;
    };

    class VulkanStaticMesh : public VulkanMeshBase
    {
    public:
        void LoadStaticMesh(std::unique_ptr<Device>& device, std::string srPath);
        
        void Bind(vk::CommandBuffer& commandBuffer);
        void Draw(vk::CommandBuffer& commandBuffer);
    private:
        void CreateVertexBuffer(std::unique_ptr<Device>& device);
        void CreateIndexBuffer(std::unique_ptr<Device>& device);
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