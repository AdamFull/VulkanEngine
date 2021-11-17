#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Device;
        class SwapChain;
        class UniformBuffer;

        struct FRendererCreateInfo
        {
            enum class ERendererType
            {
                eDeferred,
                ePostProcess
            };

            struct FAttachmentInfo
            {
                FAttachmentInfo(vk::Format f, vk::ImageUsageFlags u, vk::ClearColorValue c) :
                format(f), usage(u), color(c) {}
                vk::Format format{vk::Format::eUndefined};
                vk::ImageUsageFlags usage{};
                vk::ClearColorValue color{};
            };

            std::vector<FAttachmentInfo> vColorAttachments;
            FAttachmentInfo depthAttachment;
            ERendererType eRendererType;
            uint32_t framesInFlight;
        };

        struct FImageData
        {
            vk::Image image;
            vk::ImageView view;
            vk::ImageLayout layout;
            vk::DeviceMemory memory;
            
            void Destroy(std::shared_ptr<Device> device);
            vk::DescriptorImageInfo GetDescriptor(vk::Sampler& sampler);
        };

        using texture_type_t = Resources::ETextureAttachmentType;
        using image_map_t = std::map<texture_type_t, FImageData>;
        using attachment_t = FRendererCreateInfo::FAttachmentInfo;
        using attachments_map_t = std::map<texture_type_t, attachment_t>;
        using renderer_type_t = FRendererCreateInfo::ERendererType;
        
        namespace Rendering
        {
            class RendererBase
            {
            public:
                RendererBase() = default;
                RendererBase(std::shared_ptr<Device> device);
                ~RendererBase();

                void Create(FRendererCreateInfo createInfo);
                void ReCreate(uint32_t framesInFlight);
                void Update(vk::CommandBuffer& commandBuffer, void* data);
                void Cleanup();

            protected:
                void CreateSampler();
                FImageData CreateImage(attachment_t attachment);
                void CreateImages();
                void CreateRenderPass();
                void CreateFramebuffers();
                void CreateMaterial();

                std::vector<image_map_t> m_vImages;
                FImageData m_DepthImage;
                vk::Sampler m_Sampler;

                vk::RenderPass m_RenderPass;
                std::vector<vk::Framebuffer> m_vFramebuffers;

                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;

                //Saved attachments for recreate
                attachments_map_t m_vColorAttachments;
                attachment_t m_DepthAttachment;
                renderer_type_t m_eRendererType;
                uint32_t m_iFramesInFlight{0};

                std::shared_ptr<Device> m_device;
            };
        }
    }
}