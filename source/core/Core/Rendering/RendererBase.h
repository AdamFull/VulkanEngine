#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture { class TextureBase; }
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Device;
        class SwapChain;
        class UniformBuffer;

        namespace Renderer
        {
            //Helpers from https://stackoverflow.com/questions/771453/copy-map-values-to-vector-in-stl
            template<typename tPair>
            struct second_t 
            {
                typename tPair::second_type operator()(const tPair& p) const 
                { 
                    return p.second; 
                }
            };

            template<typename tMap> 
            second_t<typename tMap::value_type> get_map_values(const tMap& m ) 
            { 
                return second_t<typename tMap::value_type>(); 
            }

            struct FAttachmentInfo
            {
                FAttachmentInfo(vk::Format f, vk::ImageUsageFlags u, vk::ImageLayout l, vk::ImageAspectFlags a, vk::ClearColorValue c) :
                format(f), usage(u), layout(l), aspect(a), color(c) {}
                vk::Format format{vk::Format::eUndefined};
                vk::ImageUsageFlags usage{};
                vk::ImageLayout layout{};
                vk::ImageAspectFlags aspect{};
                vk::ClearColorValue color{};
            };

            struct FImage
            {
                vk::Image image;
                vk::ImageView view;
                vk::ImageLayout layout;
                vk::DeviceMemory memory;
                vk::Sampler sampler;
                vk::Format format;

                vk::DescriptorImageInfo GetDescriptor();
            };

            using gbuffer_t = std::map<Resources::ETextureAttachmentType, FImage>;

            class RendererBase
            {
            public:
                virtual void Create();
                virtual void ReCreate();
                virtual void Update(vk::CommandBuffer& commandBuffer, void* ubo);
                virtual void Cleanup();
            
            protected:
                FImage CreateImage(vk::Format format, FAttachmentInfo attachment);
                void CreateSampler();
                void CreateImages();
                virtual void CreateRenderPass();
                virtual void CreateFrameBuffers();
                virtual void CreateUniformBuffers();
                virtual void CreateMaterial();

                std::vector<gbuffer_t> m_vImages;
                FImage m_DepthImage;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<UniformBuffer> m_pUniform;
                vk::Sampler sampler;

                vk::RenderPass renderPass;
                std::vector<vk::Framebuffer> m_vFramebuffers;

                std::map<Resources::ETextureAttachmentType, FAttachmentInfo> vAttachments;
                FAttachmentInfo depthAttachment;

                std::shared_ptr<Device> m_device;
                std::shared_ptr<SwapChain> m_swapchain;
            };
        }
    }
}