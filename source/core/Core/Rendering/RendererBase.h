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
        namespace Material { class MaterialBase; }
    }
    namespace Core
    {
        class Image;
        namespace Scene
        {
            namespace Objects { class RenderObject; }
        }
        class UniformBuffer;

        struct FRendererCreateInfo
        {
            FRendererCreateInfo() = default;

            enum class ERendererType
            {
                //eShadow,
                eDeferredPBR,
                ePBRComposition,
                eGaussianRender,
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
        using image_map_t = std::map<texture_type_t, std::shared_ptr<Image>>;
        using attachment_t = FRendererCreateInfo::FAttachmentInfo;
        using attachments_map_t = std::map<texture_type_t, attachment_t>;
        using renderer_type_t = FRendererCreateInfo::ERendererType;
        
        namespace Rendering
        {
            class RendererBase : public std::enable_shared_from_this<RendererBase>
            {
            public:
                RendererBase() = default;
                virtual ~RendererBase();

                virtual void Create(std::shared_ptr<Resources::ResourceManager> pResMgr);
                virtual void Render(vk::CommandBuffer& commandBuffer);
                virtual void ReCreate(uint32_t framesInFlight);
                //void Update(vk::CommandBuffer& commandBuffer, void* data);
                virtual void Cleanup();

                inline void SetExtent(vk::Extent2D extent) { out_extent = extent; }

                //EasyDelegate::TDelegate<void(std::shared_ptr<RendererBase> pStage)> stageCB;

                std::shared_ptr<RendererBase> Find(FRendererCreateInfo::ERendererType eType);

                const std::shared_ptr<Image> GetProduct(texture_type_t eType) const;
                const image_map_t GetProducts() const;

                virtual void AddProduct(texture_type_t eAttach, std::shared_ptr<Image> image) {}
                virtual void SetUniform(void* uniform) {}

                inline void SetRenderNode(std::shared_ptr<Scene::Objects::RenderObject> pRenderObject) { m_pRenderNode = pRenderObject; }
                void SetNextStage(std::shared_ptr<RendererBase> pNext);
                void SetPrevStage(std::shared_ptr<RendererBase> pPrev);
                inline std::shared_ptr<RendererBase> GetNext() { return m_pNext; }

                inline vk::RenderPass& GetRenderPass() { return m_RenderPass; }
                inline vk::Framebuffer& GetFramebuffer(uint32_t imageIndex) { return m_vFramebuffers.at(imageIndex); }
                inline std::vector<vk::Framebuffer>& GetFramebuffers() { return m_vFramebuffers; }
                inline attachments_map_t& GetColorAttachments() { return m_vColorAttachments; }
                inline attachment_t& GetBepthAttachment() { return m_DepthAttachment; }
                inline renderer_type_t GetType() { return m_eRendererType; }
            protected:
                void CreateSampler();
                virtual std::shared_ptr<Image> CreateImage(attachment_t attachment);
                virtual void CreateImages();
                virtual void CreateRenderPass();
                virtual void CreateFramebuffers();

                void BeginRender(vk::CommandBuffer& commandBuffer);
                void EndRender(vk::CommandBuffer& commandBuffer);

                vk::Extent2D out_extent{};

                std::shared_ptr<RendererBase> m_pPrev{nullptr};
                std::shared_ptr<RendererBase> m_pNext{nullptr};

                std::vector<image_map_t> m_vImages;
                std::shared_ptr<Image> m_DepthImage;
                vk::Sampler m_Sampler;

                vk::RenderPass m_RenderPass;
                std::vector<vk::Framebuffer> m_vFramebuffers;

                std::shared_ptr<Scene::Objects::RenderObject> m_pRenderNode;

                //Saved attachments for recreate
                attachments_map_t m_vColorAttachments;
                attachment_t m_DepthAttachment;
                renderer_type_t m_eRendererType;
            };
        }
    }
}