#include "VulkanUniform.h"
#include "VulkanDevice.h"

namespace Engine
{
    void UniformBuffer::Create(std::unique_ptr<Device>& device, size_t images)
    {
        CreateUniformBuffers(device, images);
    }

    void UniformBuffer::Bind(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& layout, vk::DescriptorSet descriptorSet)
    {
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, 1, &descriptorSet, 0, nullptr);
    }

    void UniformBuffer::CreateUniformBuffers(std::unique_ptr<Device>& device, size_t images)
    {
        vk::DeviceSize bufferSize = sizeof(FUniformData);

        data.vUniformBuffers.resize(images);
        data.vUniformBuffersMemory.resize(images);

        for (size_t i = 0; i < images; i++) 
        {
            device->CreateOnDeviceBuffer
            (
                bufferSize, 
                vk::BufferUsageFlagBits::eUniformBuffer, 
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
                data.vUniformBuffers[i], 
                data.vUniformBuffersMemory[i]
            );
        }
    }

    void UniformBuffer::UpdateUniformBuffer(std::unique_ptr<Device>& device, uint32_t index, glm::mat4 transform)
    {
        FUniformData ubo{};
        ubo.transform = transform;

        device->MoveToMemory(&ubo, data.vUniformBuffersMemory[index], sizeof(ubo));
    }
}