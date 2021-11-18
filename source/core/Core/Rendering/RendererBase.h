#pragma once
#include "Resources/ResourceCunstruct.h"

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

namespace Engine
{
    namespace Resources
    {
        class ResourceManager;
        namespace Texture { class TextureBase; }
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class UniformBuffer;

        struct FRendererCreateInfo
        {
            FRendererCreateInfo() = default;

            enum class ERendererType
            {
                eDeferred,
                ePostProcess
            };

            struct FAttachmentInfo
            {
                FAttachmentInfo() = default;
                FAttachmentInfo(vk::Format f, vk::ImageUsageFlags u, vk::ClearColorValue c) :
                format(f), usage(u), color(c) {}
                vk::Format format{vk::Format::eUndefined};
                vk::ImageUsageFlags usage{};
                vk::ClearColorValue color{};
            };

            std::map<Resources::ETextureAttachmentType, FAttachmentInfo> vColorAttachments;
            FAttachmentInfo depthAttachment;
            ERendererType eRendererType;

            std::string srTexture;
            std::string srMesh;
        };

        using texture_type_t = Resources::ETextureAttachmentType;
        using image_map_t = std::map<texture_type_t, std::shared_ptr<Resources::Texture::TextureBase>>;
        using attachment_t = FRendererCreateInfo::FAttachmentInfo;
        using attachments_map_t = std::map<texture_type_t, attachment_t>;
        using renderer_type_t = FRendererCreateInfo::ERendererType;
        
        namespace Rendering
        {
            class RendererBase
            {
            public:
                RendererBase() = default;
                ~RendererBase();

                void Create(FRendererCreateInfo createInfo, std::shared_ptr<Resources::ResourceManager> pResMgr);
                std::shared_ptr<vk::RenderPassBeginInfo> CreateRenderPassCI(uint32_t imageIndex);
                void ReCreate(uint32_t framesInFlight);
                void Update(vk::CommandBuffer& commandBuffer, void* data);
                void Cleanup();

                inline vk::RenderPass& GetRenderPass() { return m_RenderPass; }
                inline vk::Framebuffer& GetFramebuffer(uint32_t imageIndex) { return m_vFramebuffers.at(imageIndex); }
                inline std::vector<vk::Framebuffer>& GetFramebuffers() { return m_vFramebuffers; }
                inline attachments_map_t& GetColorAttachments() { return m_vColorAttachments; }
                inline attachment_t& GetBepthAttachment() { return m_DepthAttachment; }
            protected:
                void CreateSampler();
                std::shared_ptr<Resources::Texture::TextureBase> CreateImage(attachment_t attachment);
                void CreateImages();
                void CreateRenderPass();
                void CreateFramebuffers();
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr);

                std::vector<image_map_t> m_vImages;
                std::shared_ptr<Resources::Texture::TextureBase> m_DepthImage;
                vk::Sampler m_Sampler;

                vk::RenderPass m_RenderPass;
                std::vector<vk::Framebuffer> m_vFramebuffers;

                std::shared_ptr<UniformBuffer> m_pUniform;
                std::shared_ptr<Resources::Material::MaterialBase> m_pMaterial;

                //Saved attachments for recreate
                attachments_map_t m_vColorAttachments;
                attachment_t m_DepthAttachment;
                renderer_type_t m_eRendererType;
            };
        }
    }
}