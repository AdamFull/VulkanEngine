#include "VulkanTexture.h"

namespace Engine
{
    void VulkanTextureBase::UpdateDescriptor()
    {
        descriptor.sampler = sampler;
		descriptor.imageView = view;
		descriptor.imageLayout = imageLayout;
    }

    void VulkanTextureBase::Destroy(vk::UniqueDevice& device)
    {
        device->destroySampler(sampler);
        device->destroyImageView(view);
        device->destroyImage(image);
        device->freeMemory(deviceMemory);
    }
}