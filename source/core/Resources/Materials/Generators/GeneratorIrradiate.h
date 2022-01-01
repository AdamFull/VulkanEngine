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