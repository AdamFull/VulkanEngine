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
                FAttachmentInfo(vk::Format f, vk::ImageUsageFlags u, vk::ClearColorValue c) :
                format(f), usage(u), color(c) {}
                vk::Format format{vk::Format::eUndefined};
                vk::ImageUsageFlags usage{};
                vk::ClearColorValue color{};
            };

            using gbuffer_t = std::map<Resources::ETextureAttachmentType, std::shared_ptr<Resources::Texture::TextureBase>>;

            class RendererBase
            {
            public:
                virtual void Create();
                virtual void ReCreate();
                virtual void Update(vk::CommandBuffer& commandBuffer, void* ubo);
                virtual void Cleanup();
            
            protected:
                std::shared_ptr<Resources::Texture::TextureBase> CreateImage(vk::Format format, vk::ImageUsageFlags usage);
                void CreateSampler();
                void CreateImages();
                virtual void CreateRenderPass();
                virtual void CreateFrameBuffers();
                virtual void CreateUniformBuffers();
                virtual void CreateMaterial();

                std::vector<gbuffer_t> m_vImages;
                std::shared_ptr<Resources::Texture::TextureBase> m_pDepthImage;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;
                std::shared_ptr<UniformBuffer> m_pUniform;
                vk::Sampler sampler;

                vk::RenderPass renderPass;
                std::vector<vk::Framebuffer> m_vFramebuffers;

                std::map<Resources::ETextureAttachmentType, FAttachmentInfo> vAttachments;

                std::shared_ptr<Device> m_device;
                std::shared_ptr<SwapChain> m_swapchain;
            };
        }
    }
}