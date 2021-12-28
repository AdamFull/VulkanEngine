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
                struct FPrefiltredPushBlock 
                {
                    glm::mat4 mvp;
			        float roughness;
			        uint32_t numSamples = 32u;
                };

                class GeneratorPrefiltred : public GeneratorBase
                {
                public:
                    GeneratorPrefiltred() = default;

                    void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                    void Generate(std::shared_ptr<Mesh::MeshBase> pMesh) override;
                    std::shared_ptr<Texture::Image> Get() override;
                protected:
                    void CreateDescriptors(uint32_t images) override;
                    void CreateTextures() override;
                private:
                    std::shared_ptr<Texture::Image> m_pCubemap;
                };
            }
        }
    }
}