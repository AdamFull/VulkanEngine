#pragma once

namespace Engine
{
    struct FVulkanModelInfo
    {
        std::vector<Types::Vertex> vVertices;
        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;

        std::vector<uint32_t> vIndices;
        vk::Buffer indexBuffer;
        vk::DeviceMemory indiciesBufferMemory;
    };
    
}