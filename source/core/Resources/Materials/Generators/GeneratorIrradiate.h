#pragma once
#include "GeneratorBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            namespace Generator
            {
                struct FIrradiatePushBlock 
                {
                    glm::mat4 mvp;
                    // Sampling deltas
                    float deltaPhi = (2.0f * float(3.14)) / 180.0f;
                    float deltaTheta = (0.5f * float(3.14)) / 64.0f;
                };

                class GeneratorIrradiate : public GeneratorBase
                {
                public:
                    GeneratorIrradiate() = default;
                    GeneratorIrradiate(std::shared_ptr<Core::Device> device, std::shared_ptr<Core::SwapChain> swapchain);

                    void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                    void Generate(std::shared_ptr<Mesh::MeshBase> pMesh) override;
                    std::shared_ptr<Texture::TextureBase> Get() override;
                protected:
                    inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eIrradiateCube; }
                    void CreateDescriptors(uint32_t images) override;
                    void CreateTextures() override;

                private:
                    std::shared_ptr<Texture::TextureBase> m_pCubemap;
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