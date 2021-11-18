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
                class GeneratorBRDF : public GeneratorBase
                {
                public:
                    GeneratorBRDF() = default;

                    void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                    void Generate(std::shared_ptr<Mesh::MeshBase> pMesh) override;
                protected:
                    inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eBRDF; }
                    void CreateDescriptors(uint32_t images) override;
                    void CreateTextures() override;
                };
            }
        }
    }
}