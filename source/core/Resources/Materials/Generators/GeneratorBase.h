#pragma once
#include "Resources/Materials/VulkanMaterial.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh { class MeshBase; }
        namespace Material
        {
            namespace Generator
            {
                class GeneratorBase : public MaterialBase
                {
                public:
                    GeneratorBase() = default;
                    ~GeneratorBase() override;

                    void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                    void Cleanup() override;

                    virtual void Generate(std::shared_ptr<Mesh::MeshBase> pMesh);
                    virtual std::shared_ptr<Texture::TextureBase> Get();
                protected:
                    virtual inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eNone; }
                    Core::Pipeline::FPipelineCreateInfo CreateInfo(Core::Pipeline::EShaderSet eSet) override;
                    virtual void CreateDescriptors(uint32_t images) override;
                    void CreateRenderPass(vk::Format format);
                    void CreateFramebuffer();
                    virtual void CreateTextures();

                protected:
                    std::shared_ptr<Texture::TextureBase> m_pGeneratedImage;
                    vk::Framebuffer framebuffer;
                    vk::Format imageFormat;
                    uint32_t m_iDimension{1024};
                    uint32_t m_iInternalFormat{0x822F};

                    const std::vector<glm::mat4> matrices
                    {
                        // POSITIVE_X
                        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                        // NEGATIVE_X
                        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                        // POSITIVE_Y
                        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                        // NEGATIVE_Y
                        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                        // POSITIVE_Z
                        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                        // NEGATIVE_Z
                        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                    };
                };
            }
        }
    }
}