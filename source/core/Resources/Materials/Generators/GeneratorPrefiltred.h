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
                    std::shared_ptr<Core::Image> Get() override;
                protected:
                    void CreateTextures() override;
                private:
                    std::shared_ptr<Core::Image> m_pCubemap;
                };
            }
        }
    }
}